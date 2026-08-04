#pragma once

#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/ui/ui_context.hpp"
#include "engine/ui/ui_manager.hpp"

namespace ls {

  class IScene {
  public:
    virtual ~IScene() = default;

    virtual void onEnter() = 0;
    virtual void onExit() = 0;

    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void onResize(int width, int height) = 0;

    virtual bool isOpaque() const { return true; }

  protected:
    ui::UIContext getUIContext() {
      return ui::UIContext{
        .registry = registry_,
        .eventQueue = eventQueue_,
        .textureManager = textureManager_,
      };
    }

    ecs::Registry registry_;
    dispatch::EventQueue eventQueue_;
    ui::UIManager uiManager_;
    gfx::TextureManager textureManager_;
    input::InputManager inputManager_;
    input::InputManager debugInputManager_;
  };

}  // namespace ls
