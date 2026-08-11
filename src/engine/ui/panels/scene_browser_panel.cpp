#include "engine/ui/panels/scene_browser_panel.hpp"

#include <imgui.h>

#include "engine/core/scene_manager.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_change_scene.hpp"

namespace ls::ui {

  void SceneBrowserPanel::render(const UIContext& ctx) {
    if (!ctx.engineCtx.sceneManager) {
      return;
    }

    if (ImGui::Begin(
            "Scene Browser", &visible_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
        )) {
      for (const auto& name : ctx.engineCtx.sceneManager->getRegisteredSceneNames()) {
        if (ImGui::Button(name.c_str(), ImVec2(200.f, 0.f))) {
          ctx.eventQueue.publish(event::RequestChangeScene{ .name = name });
        }
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
