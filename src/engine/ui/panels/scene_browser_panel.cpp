#include "engine/ui/panels/scene_browser_panel.hpp"

#include <imgui.h>

#include <cassert>

#include "engine/core/scene_manager.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_change_scene.hpp"

namespace ls::ui {

  void SceneBrowserPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.sceneManager != nullptr && "SceneBrowserPanel requires a valid SceneManager!");

    if (ImGui::Begin(
            "Scene Browser", &visible_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
        )) {
      for (const auto& name : ctx.engineCtx.sceneManager->getRegisteredSceneNames()) {
        if (ImGui::Button(name.c_str(), ImVec2(200.f, 0.f))) {
          ctx.engineCtx.eventQueue->publish(event::RequestChangeScene{ .name = name });
        }
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
