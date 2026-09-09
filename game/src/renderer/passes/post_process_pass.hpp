#pragma once

#include <cstdint>

#include "ls/gfx/framebuffer.hpp"
#include "ls/renderer/i_render_pass.hpp"

namespace ls::renderer {

  class PostProcessPass : public IRenderPass {
  public:
    PostProcessPass() = default;
    ~PostProcessPass() = default;

    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void execute(const RenderContext& ctx) override;

    const char* getName() const override { return "Post Process Pass"; }

  private:
    gfx::Framebuffer postProcessFramebuffer_{};

    uint32_t vao_{ 0 };
    uint32_t vbo_{ 0 };
  };

}  // namespace ls::renderer
