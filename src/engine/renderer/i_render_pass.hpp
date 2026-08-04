#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <utility>

#include "engine/ecs/registry.hpp"
#include "engine/gfx/framebuffer.hpp"
#include "engine/gfx/texture_manager.hpp"

namespace ls::renderer {

  struct RenderContext {
    const ecs::Registry& registry;
    const glm::mat4& viewProjection{ 1.f };
    const gfx::TextureManager& textureManager;
  };

  class IRenderPass {
  public:
    IRenderPass(std::shared_ptr<gfx::Framebuffer> targetFBO = nullptr)
        : targetFBO_{ std::move(targetFBO) } {}

    virtual ~IRenderPass() = default;

    virtual void onEnter() = 0;
    virtual void execute(const RenderContext& ctx) = 0;

    virtual const char* getName() const = 0;

    virtual gfx::Framebuffer* getTargetFBO() const { return targetFBO_.get(); }

  protected:
    std::shared_ptr<gfx::Framebuffer> targetFBO_{ nullptr };
  };

}  // namespace ls::renderer
