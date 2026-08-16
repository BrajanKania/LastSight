#include "engine/ui/panels/scene_browser_panel.hpp"

#include <imgui.h>

#include <cassert>

#include "engine/core/scene_manager.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_change_scene.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  void SceneBrowserPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.sceneManager != nullptr && "SceneBrowserPanel requires a valid SceneManager!");

    ImGuiWindowFlags windowFlags{ ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing };

    if (ImGui::Begin(ui::panel::kSceneBrowser, &visible_, windowFlags)) {
      for (const auto& name : ctx.engineCtx.sceneManager->getRegisteredSceneNames()) {
        if (ImGui::Button(name.c_str(), ImVec2(200.f, 0.f))) {
          ctx.engineCtx.eventQueue->publish(event::RequestChangeScene{ .name = name });
        }
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
