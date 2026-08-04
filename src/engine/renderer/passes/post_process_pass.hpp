#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "engine/gfx/framebuffer.hpp"
#include "engine/gfx/shader.hpp"
#include "engine/renderer/i_render_pass.hpp"

namespace ls::renderer {

  class PostProcessPass : public IRenderPass {
  public:
    PostProcessPass(std::shared_ptr<gfx::Framebuffer> target, std::shared_ptr<gfx::Framebuffer> source);

    void onEnter() override;
    void execute(const RenderContext& ctx) override;
    const char* getName() const override { return "Post Process Pass"; }

  private:
    std::shared_ptr<gfx::Framebuffer> sourceFBO_;
    std::optional<gfx::Shader> shader_;

    uint32_t vao_{ 0 };
    uint32_t vbo_{ 0 };
  };

}  // namespace ls::renderer
