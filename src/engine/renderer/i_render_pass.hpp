#pragma once

#include <glm/glm.hpp>

#include "engine/renderer/render_context.hpp"

namespace ls::renderer {

  class IRenderPass {
  public:
    IRenderPass() = default;
    virtual ~IRenderPass() = default;

    virtual void onEnter() = 0;
    virtual void onExit() = 0;
    virtual void onResize(int width, int height) = 0;
    virtual void execute(const RenderContext& ctx) = 0;

    virtual const char* getName() const = 0;
  };

}  // namespace ls::renderer
