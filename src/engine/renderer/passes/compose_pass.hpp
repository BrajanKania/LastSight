#pragma once

#include <memory>
#include <optional>

#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/shader.hpp"

namespace ls {

  class ComposePass : public IRenderPass {
  public:
    ComposePass(std::shared_ptr<Framebuffer> processedFBO);
    void onEnter() override;
    void execute(const RenderContext& ctx) override;

  private:
    std::shared_ptr<Framebuffer> processedFBO_;

    std::optional<Shader> shader_;

    unsigned int vao_{0};
    unsigned int vbo_{0};
  };

}  // namespace ls
