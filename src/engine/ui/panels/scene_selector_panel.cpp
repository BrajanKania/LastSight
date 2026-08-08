#include "engine/ui/panels/scene_selector_panel.hpp"

#include <imgui.h>

#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_change_scene.hpp"

namespace ls::ui {

  SceneSelectorPanel::SceneSelectorPanel(const SceneManager& sceneManager)
      : sceneManager_(sceneManager) {}

  void SceneSelectorPanel::render(const UIContext& ctx) {
    if (ImGui::Begin(
            "Scene Selector", &visible_, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing
        )) {
      for (const auto& name : sceneManager_.getRegisteredSceneNames()) {
        if (ImGui::Button(name.c_str(), ImVec2(200.f, 0.f))) {
          ctx.eventQueue.publish(event::RequestChangeScene{ .name = name });
        }
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
