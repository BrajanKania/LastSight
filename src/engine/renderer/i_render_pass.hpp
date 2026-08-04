#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "engine/ecs/registry.hpp"
#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/texture_manager.hpp"

namespace ls {

  struct RenderContext {
    const ecs::Registry& registry;
    const glm::mat4& viewProjection{ 1.f };
    const TextureManager& textureManager;
  };

  class IRenderPass {
  public:
    virtual ~IRenderPass() = default;
    virtual void onEnter() = 0;
    virtual void execute(const RenderContext& ctx) = 0;
    virtual const char* getName() const = 0;
    virtual Framebuffer* getTargetFBO() { return nullptr; }

  protected:
    std::shared_ptr<Framebuffer> targetFBO_{ nullptr };
  };

}  // namespace ls
