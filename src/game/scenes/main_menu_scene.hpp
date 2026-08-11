#pragma once

#include "engine/core/i_scene.hpp"
#include "engine/dispatch/event_queue.hpp"

namespace ls {

  class MainMenuScene : public IScene {
  public:
    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void handleInput() override;
    void update(float dt) override;
    void render() override;

    bool isOpaque() const override { return true; }

    SceneContext getSceneContext() override { return SceneContext{}; }

  private:
    ui::UIContext getUIContext() override {
      return ui::UIContext{
        .eventQueue = eventQueue_,
      };
    }

    dispatch::EventQueue eventQueue_;
  };

}  // namespace ls
