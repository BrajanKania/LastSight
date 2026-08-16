#pragma once

#include "engine/core/engine_context.hpp"
#include "engine/core/scene_context.hpp"
#include "engine/ui/ui_context.hpp"

namespace ls {

  class IScene {
  public:
    explicit IScene(EngineContext engineCtx)
        : engineCtx_{ engineCtx } {}

    virtual ~IScene() = default;

    virtual void onEnter() = 0;
    virtual void onExit() = 0;

    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    virtual void onResize(int width, int height) = 0;

    virtual bool isOpaque() const { return true; }

    virtual SceneContext getSceneContext() = 0;

  protected:
    virtual void processEvents() = 0;

    virtual ui::UIContext getUIContext() {
      return ui::UIContext{
        .engineCtx = engineCtx_,
        .sceneCtx = getSceneContext(),
      };
    }

    EngineContext engineCtx_;
  };

}  // namespace ls
