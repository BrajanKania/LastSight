#pragma once

#include <memory>
#include <optional>

#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/shader.hpp"

namespace ls {

  class LitPass : public IRenderPass {
  public:
    LitPass(std::shared_ptr<Framebuffer> target);
    void onEnter() override;
    void execute(const RenderContext& ctx) override;
    const char* getName() const override { return "Lit Pass"; }

    Framebuffer* getTargetFBO() override { return targetFBO_.get(); }

  private:
    std::shared_ptr<Framebuffer> targetFBO_;
    std::optional<Shader> shader_;
    unsigned int vao_{ 0 };
    unsigned int vbo_{ 0 };
  };

}  // namespace ls
