#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "engine/gfx/framebuffer.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/renderer/i_render_pass.hpp"

namespace ls::renderer {

  class ComposePass : public IRenderPass {
  public:
    ComposePass(std::shared_ptr<gfx::Framebuffer> target);

    void onEnter() override;
    void execute(const RenderContext& ctx) override;
    const char* getName() const override { return "Compose Pass"; }

  private:
    std::optional<gfx::Shader> shader_;

    uint32_t vao_{ 0 };
    uint32_t vbo_{ 0 };
  };

}  // namespace ls::renderer
