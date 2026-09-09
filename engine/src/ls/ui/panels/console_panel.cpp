#include "ls/ui/panels/console_panel.hpp"

#include <imgui.h>

#include <cassert>
#include <format>

#include "ls/core/engine_context.hpp"
#include "ls/debug/console.hpp"
#include "ls/debug/log_level.hpp"
#include "ls/ui/panels/panel_names.hpp"
#include "ls/ui/ui_colors.hpp"

namespace ls::ui {

  namespace {

    struct ConsoleInputUserData {
      debug::Console* console{ nullptr };
    };

    int textEditCallback(ImGuiInputTextCallbackData* data) {
      auto* userData{ static_cast<ConsoleInputUserData*>(data->UserData) };
      auto* console{ userData->console };

      if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion) {
        std::string input(data->Buf, data->CursorPos);

        std::vector<std::string> matches;
        for (const auto& [name, info] : console->getCommands()) {
          if (name.starts_with(input)) {
            matches.push_back(name);
          }
        }

        if (matches.empty()) {
          console->log(std::format("No match for '{}'", input), debug::LogLevel::Warning);
        } else if (matches.size() == 1) {
          std::string completion{ matches[0] + " " };
          data->DeleteChars(0, data->BufTextLen);
          data->InsertChars(0, completion.c_str());
        } else {
          std::string matchPrefix{ matches[0] };
          for (size_t i{ 1 }; i < matches.size(); ++i) {
            size_t j{ 0 };
            while (j < matchPrefix.size() && j < matches[i].size() && matchPrefix[j] == matches[i][j]) {
              j++;
            }
            matchPrefix = matchPrefix.substr(0, j);
          }

          if (matchPrefix.size() > input.size()) {
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, matchPrefix.c_str());
          }

          console->log(std::format("> {}", input), debug::LogLevel::Info);
          console->log("Possible completions:", debug::LogLevel::Info);
          for (const auto& match : matches) {
            const auto& info{ console->getCommands().at(match) };
            console->log(std::format(" {}", info.description), debug::LogLevel::Info);
          }
        }
      }
      return 0;
    }

  }  // namespace

  void ConsolePanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.console != nullptr && "[ConsolePanel] Requires a valid Console!");
    assert(ctx.editorPreferences != nullptr && "[ConsolePanel] Requires a valid EditorPreferences");

    if (!ImGui::Begin(ui::panel::kConsole)) {
      ImGui::End();
      return;
    }

    const float footerHeightToReserve{ ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() };

    if (ImGui::BeginChild(
            "ScrollRegion", ImVec2(0, -footerHeightToReserve), false, ImGuiWindowFlags_HorizontalScrollbar
        )) {
      const bool isAtBottom{ ImGui::GetScrollY() >= (ImGui::GetScrollMaxY() - 5.0f) };

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

      for (const auto& log : ctx.engineCtx.console->getLogs()) {
        ImGui::PushStyleColor(ImGuiCol_Text, color::forLogLevel(log.level));
        ImGui::TextUnformatted(log.message.c_str());
        ImGui::PopStyleColor();
      }

      if (ctx.editorPreferences->consoleAutoScroll && isAtBottom) {
        ImGui::SetScrollHereY(1.0f);
      }

      ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::Separator();

    ImGuiInputTextFlags inputFlags{ ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion };

    if (reclaimFocus_) {
      ImGui::SetKeyboardFocusHere();
      reclaimFocus_ = false;
    }

    ConsoleInputUserData userData{ .console = ctx.engineCtx.console };

    ImGui::PushItemWidth(-1.0f);
    if (ImGui::InputTextWithHint(
            "##ConsoleInput",
            "Enter command...",
            inputBuffer_,
            sizeof(inputBuffer_),
            inputFlags,
            textEditCallback,
            &userData
        )) {
      if (inputBuffer_[0] != '\0') {
        ctx.engineCtx.console->execute(inputBuffer_);
        inputBuffer_[0] = '\0';
      }
      reclaimFocus_ = true;
    }
    ImGui::PopItemWidth();

    ImGui::End();
  }

}  // namespace ls::ui
