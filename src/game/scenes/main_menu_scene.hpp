#pragma once

#include "engine/core/i_scene.hpp"

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
  };

}  // namespace ls
