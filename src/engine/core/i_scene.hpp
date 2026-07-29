#pragma once

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
  };

}  // namespace ls
