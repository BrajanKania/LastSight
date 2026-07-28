#pragma once

#include <memory>
#include <optional>

#include "renderer/framebuffer.hpp"
#include "renderer/render_pass.hpp"
#include "renderer/shader.hpp"

namespace ls {

  class PostProcessPass : public IRenderPass {
  public:
    PostProcessPass(std::shared_ptr<Framebuffer> processedFBO, std::shared_ptr<Framebuffer> worldFBO);
    void onEnter() override;
    void execute(const RenderContext& ctx) override;

  private:
    std::shared_ptr<Framebuffer> processedFBO_;
    std::shared_ptr<Framebuffer> worldFBO_;

    std::optional<Shader> shader_;

    unsigned int vao_{0};
    unsigned int vbo_{0};
  };

}  // namespace ls
