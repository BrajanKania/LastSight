#pragma once

#include <cstdint>

#include "engine/gfx/framebuffer.hpp"
#include "engine/renderer/i_render_pass.hpp"

namespace ls::renderer {

  class FovPass : public IRenderPass {
  public:
    FovPass() = default;

    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void execute(const RenderContext& ctx) override;

    const char* getName() const override { return "Fov Pass"; }

  private:
    gfx::Framebuffer fovFramebuffer_{};

    uint32_t vao_{ 0 };
    uint32_t vbo_{ 0 };
  };

}  // namespace ls::renderer
