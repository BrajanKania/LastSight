#pragma once

#include <cstdint>

#include "ls/gfx/framebuffer.hpp"
#include "ls/renderer/i_render_pass.hpp"

namespace ls::renderer {

  class DefaultLitPass : public IRenderPass {
  public:
    DefaultLitPass() = default;

    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void execute(const RenderContext& ctx) override;

    const char* getName() const override { return "Default Lit Pass"; }

  private:
    gfx::Framebuffer litFBO_{};
    std::uint32_t vao_{ 0 };
    std::uint32_t vbo_{ 0 };
  };

}  // namespace ls::renderer
