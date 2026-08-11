#pragma once

#include "engine/core/engine_context.hpp"
#include "engine/core/scene_context.hpp"
#include "engine/core/scene_manager.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/ui/ui_manager.hpp"

namespace ls {

  class EditorLayer {
  public:
    EditorLayer();

    void handleInput(dispatch::EventQueue& engineEventQueue, bool blockKeyboard, bool blockMouse);
    void update(dispatch::EventQueue& engineEventQueue, SceneManager& sceneManager);
    void render(dispatch::EventQueue& engineEventQueue, const EngineContext& engineCtx, const SceneContext& sceneCtx);

  private:
    void handleRequest(dispatch::EventQueue& engineEventQueue);

    ui::UIManager uiManager_;
    input::InputManager inputManager_;
  };

}  // namespace ls
