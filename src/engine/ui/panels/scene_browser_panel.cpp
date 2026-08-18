#include "engine/ui/panels/scene_browser_panel.hpp"

#include <imgui.h>

#include <cassert>
#include <string>
#include <string_view>

#include "engine/core/scene_manager.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_change_scene.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  void SceneBrowserPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.sceneManager != nullptr && "[SceneBrowserPanel] requires a valid SceneManager!");

    if (ImGui::Begin(ui::panel::kSceneBrowser, &visible_)) {
      ImGui::SetNextItemWidth(200.0f);
      ImGui::InputTextWithHint("##SceneFilter", "Search scenes...", filterBuffer_, sizeof(filterBuffer_));

      ImGui::SameLine();
      ImGui::TextDisabled("(Total: %zu)", ctx.engineCtx.sceneManager->getRegisteredSceneNames().size());

      ImGui::Separator();

      const float cardWidth{ 110.0f };
      const float cardHeight{ 55.0f };
      const float availWidth{ ImGui::GetContentRegionAvail().x };

      int columns{ static_cast<int>(availWidth / (cardWidth + 12.0f)) };
      if (columns < 1)
        columns = 1;

      if (ImGui::BeginTable("SceneBrowserGrid", columns)) {
        const std::string_view activeScene{ ctx.engineCtx.sceneManager->getActiveSceneName() };

        for (const auto& name : ctx.engineCtx.sceneManager->getRegisteredSceneNames()) {
          if (filterBuffer_[0] != '\0' && name.find(filterBuffer_) == std::string::npos) {
            continue;
          }

          ImGui::TableNextColumn();
          ImGui::PushID(name.c_str());

          const bool isActive{ name == activeScene };
          const ImVec2 screenPos{ ImGui::GetCursorScreenPos() };
          const ImVec2 cardMin{ screenPos };
          const ImVec2 cardMax{ screenPos.x + cardWidth, screenPos.y + cardHeight };

          ImDrawList* drawList{ ImGui::GetWindowDrawList() };
          drawList->ChannelsSplit(2);

          drawList->ChannelsSetCurrent(1);
          ImGui::BeginGroup();

          ImGui::SetCursorScreenPos(ImVec2(screenPos.x + 8.0f, screenPos.y + 8.0f));
          ImGui::PushTextWrapPos(screenPos.x + cardWidth - 8.0f);
          ImGui::TextUnformatted(name.c_str());
          ImGui::PopTextWrapPos();

          ImGui::SetCursorScreenPos(screenPos);
          ImGui::Dummy(ImVec2(cardWidth, cardHeight));

          ImGui::EndGroup();

          const bool isCardHovered{ ImGui::IsItemHovered() };

          if (isCardHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            if (!isActive) {
              ctx.engineCtx.eventQueue->publish(
                  event::RequestChangeScene{
                      .name = name,
                  }
              );
            }
          }

          drawList->ChannelsSetCurrent(0);
          ImU32 bgColor;
          if (isActive) {
            bgColor = isCardHovered ? ImGui::GetColorU32(ImVec4(0.22f, 0.45f, 0.26f, 1.0f))
                                    : ImGui::GetColorU32(ImVec4(0.18f, 0.35f, 0.22f, 1.0f));
          } else {
            bgColor = isCardHovered ? ImGui::GetColorU32(ImVec4(0.28f, 0.28f, 0.28f, 1.0f))
                                    : ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
          }

          drawList->AddRectFilled(cardMin, cardMax, bgColor, 6.0f);

          if (isActive) {
            drawList->AddRect(cardMin, cardMax, ImGui::GetColorU32(ImVec4(0.35f, 0.70f, 0.40f, 1.0f)), 6.0f, 0, 1.5f);
          }

          drawList->ChannelsMerge();
          ImGui::PopID();
        }
        ImGui::EndTable();
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
