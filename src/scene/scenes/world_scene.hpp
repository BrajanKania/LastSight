#pragma once

#include "core/ecs.hpp"
#include "renderer/render_pipeline.hpp"
#include "scene/scene.hpp"

namespace ls {

  class WorldScene : public IScene {
  public:
    void onEnter() override;
    void onExit() override;

    void handleInput() override;
    void update(float dt) override;
    void render() override;

    bool isOpaque() const override { return true; }

  private:
    ecs::Registry registry_;
    RenderPipeline renderPipeline_;
  };

}  // namespace ls
