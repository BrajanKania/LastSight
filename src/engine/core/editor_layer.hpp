#pragma once

#include "engine/core/engine_context.hpp"
#include "engine/core/scene_context.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/ui/selection_context.hpp"
#include "engine/ui/ui_manager.hpp"

namespace ls {

  class EditorLayer {
  public:
    EditorLayer();

    void handleInput(EngineContext engineCtx, bool blockKeyboard, bool blockMouse);
    void update(EngineContext engineCtx);
    void render(EngineContext engineCtx, const SceneContext& sceneCtx);

    bool isViewportHovered() const;
    bool isViewportFocused() const;

  private:
    void handleRequest(dispatch::EventQueue& engineEventQueue);
    void renderDockSpace();

    bool shouldResetLayout_{ true };

    ui::UIManager uiManager_;
    input::InputManager inputManager_;
    ui::SelectionContext selectionCtx_;
  };

}  // namespace ls
