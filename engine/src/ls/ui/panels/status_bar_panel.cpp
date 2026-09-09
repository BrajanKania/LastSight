#include "ls/ui/panels/status_bar_panel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "ls/debug/status_bar.hpp"
#include "ls/ui/ui_colors.hpp"

namespace ls::ui {

  void StatusBarPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.statusBar != nullptr && "[StatusBarPanel] Requires a valid StatusBar!");

    ImGuiViewport* viewport{ ImGui::GetMainViewport() };
    ImGuiWindowFlags flags{ ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                            ImGuiWindowFlags_MenuBar };

    if (ImGui::BeginViewportSideBar("##MainStatusBar", viewport, ImGuiDir_Down, ImGui::GetFrameHeight(), flags)) {
      if (ImGui::BeginMenuBar()) {
        const auto& notification{ ctx.engineCtx.statusBar->getNotification() };

        const ImVec4 color{ (notification.timeRemaining > 0.0f) ? ui::color::forLogLevel(notification.level)
                                                                : ImVec4(0.5f, 0.5f, 0.5f, 1.0f) };

        ImGui::TextColored(color, "%s", notification.message.c_str());

        float rightSectionWidth{ 80.0f };
        ImGui::SameLine(ImGui::GetWindowWidth() - rightSectionWidth);

        float fps{ ImGui::GetIO().Framerate };

        ImGui::TextDisabled("FPS: %.0f", fps);

        ImGui::EndMenuBar();
      }
      ImGui::End();
    }
  }
}  // namespace ls::ui
