#pragma once

#include <memory>

#include "engine/core/i_scene.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/renderer/texture_manager.hpp"

namespace ls {

  class WorldScene : public IScene {
  public:
    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void handleInput() override;
    void update(float dt) override;
    void render() override;

    bool isOpaque() const override { return true; }

  private:
    ecs::Registry registry_;
    RenderPipeline renderPipeline_;
    TextureManager textureManager_;

    std::shared_ptr<Framebuffer> worldFBO_{ nullptr };
    std::shared_ptr<Framebuffer> fovFBO_{ nullptr };
    std::shared_ptr<Framebuffer> processedFBO_{ nullptr };

    ecs::Entity player_{ 0 };
  };

}  // namespace ls
