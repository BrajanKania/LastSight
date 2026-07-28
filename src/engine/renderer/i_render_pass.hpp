#pragma once

#include "engine/ecs/registry.hpp"

namespace ls {

  struct RenderContext {
    const ecs::Registry& registry;
  };

  class IRenderPass {
  public:
    virtual ~IRenderPass() = default;
    virtual void onEnter() = 0;
    virtual void execute(const RenderContext& ctx) = 0;
  };

}  // namespace ls
