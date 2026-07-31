#pragma once

#include <memory>
#include <optional>

#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/shader.hpp"

namespace ls {

  class FovPass : public IRenderPass {
  public:
    FovPass(std::shared_ptr<Framebuffer> target, std::shared_ptr<Framebuffer> source);
    void onEnter() override;
    void execute(const RenderContext& ctx) override;

  private:
    std::shared_ptr<Framebuffer> targetFBO_;
    std::shared_ptr<Framebuffer> sourceFBO_;
    std::optional<Shader> shader_;
    unsigned int vao_{ 0 };
    unsigned int vbo_{ 0 };
  };

}  // namespace ls
