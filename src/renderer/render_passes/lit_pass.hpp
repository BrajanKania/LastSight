#pragma once

#include <optional>

#include "renderer/render_pass.hpp"
#include "renderer/shader.hpp"

namespace ls {

  class LitPass : public IRenderPass {
  public:
    void onEnter() override;
    void execute(const RenderContext& ctx) override;

  private:
    std::optional<Shader> shader_;
    unsigned int vao_;
    unsigned int vbo_;
  };

}  // namespace ls
