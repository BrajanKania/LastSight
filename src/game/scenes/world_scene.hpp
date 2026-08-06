#pragma once

#include <memory>

#include "engine/core/i_scene.hpp"
#include "engine/ecs/types.hpp"
#include "engine/gfx/framebuffer.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "game/items/item_registry.hpp"

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
    renderer::RenderPipeline renderPipeline_;
    item::ItemRegistry itemRegistry_;

    std::shared_ptr<gfx::Framebuffer> worldFBO_{ nullptr };
    std::shared_ptr<gfx::Framebuffer> fovFBO_{ nullptr };
    std::shared_ptr<gfx::Framebuffer> processedFBO_{ nullptr };

    ecs::EntityId player_{ 0 };
  };

}  // namespace ls
