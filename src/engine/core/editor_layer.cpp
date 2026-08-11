#include "engine/core/editor_layer.hpp"

#include "engine/actions/toggle_debug.hpp"
#include "engine/core/engine_context.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/events/set_panel_visibility.hpp"
#include "engine/events/toggle_panel.hpp"
#include "engine/input/types.hpp"
#include "engine/ui/panels/entity_inspector_panel.hpp"
#include "engine/ui/panels/panel_names.hpp"
#include "engine/ui/panels/render_pipeline_debug_panel.hpp"
#include "engine/ui/panels/scene_browser_panel.hpp"
#include "engine/ui/panels/toolbox_panel.hpp"

namespace ls {

  EditorLayer::EditorLayer() {
    inputManager_.bindKey<action::ToggleDebug>(input::Key::Grave);

    uiManager_.addPanel<ui::ToolboxPanel>(ui::panel::kToolbox);
    uiManager_.addPanel<ui::RenderPipelineDebugPanel>(ui::panel::kRenderPipelineDebug);
    uiManager_.addPanel<ui::EntityInspectorPanel>(ui::panel::kEntityInspector);
    uiManager_.addPanel<ui::SceneBrowserPanel>(ui::panel::kSceneBrowser);
  }

  void EditorLayer::handleInput(dispatch::EventQueue& engineEventQueue, bool blockKeyboard, bool blockMouse) {
    inputManager_.update(blockKeyboard, blockMouse);

    auto toggleDebugActionState{ inputManager_.getActionState<action::ToggleDebug>() };
    if (toggleDebugActionState == input::ActionState::JustPressed) {
      engineEventQueue.publish<event::TogglePanel>(event::TogglePanel{
          .name = ui::panel::kToolbox,
      });

      engineEventQueue.publish(
          event::SetPanelVisibility{
              .name = ui::panel::kSceneBrowser,
              .visible = false,
          }
      );

      engineEventQueue.publish(
          event::SetPanelVisibility{
              .name = ui::panel::kEntityInspector,
              .visible = false,
          }
      );

      engineEventQueue.publish(
          event::SetPanelVisibility{
              .name = ui::panel::kRenderPipelineDebug,
              .visible = false,
          }
      );
    }
  }

  void EditorLayer::update(dispatch::EventQueue& engineEventQueue, SceneManager& sceneManager) {
    handleRequest(engineEventQueue);
  }

  void EditorLayer::render(
      dispatch::EventQueue& engineEventQueue, const EngineContext& engineCtx, const SceneContext& sceneCtx
  ) {
    uiManager_.render(
        ui::UIContext{
            .eventQueue = engineEventQueue,
            .engineCtx = engineCtx,
            .sceneCtx = sceneCtx,
        }
    );
  }

  void EditorLayer::handleRequest(dispatch::EventQueue& engineEventQueue) {
    for (const auto& event : engineEventQueue.getEvents<event::TogglePanel>()) {
      uiManager_.getPanel(event.name).toggleVisible();
    }

    for (const auto& event : engineEventQueue.getEvents<event::SetPanelVisibility>()) {
      uiManager_.getPanel(event.name).setVisible(event.visible);
    }
  }

}  // namespace ls
