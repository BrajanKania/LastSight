#include "game/renderer/passes/post_process_pass.hpp"

#include <glad/gl.h>

#include <glm/common.hpp>
#include <glm/ext/scalar_common.hpp>

#include "engine/renderer/material/material_names.hpp"
#include "engine/renderer/render_system.hpp"

namespace ls::renderer {

  void PostProcessPass::onEnter() {
    // clang-format off
    float vertex[] {
      0.f, 1.f,
      0.f, 0.f,
      1.f, 0.f,

      0.f, 1.f,
      1.f, 0.f,
      1.f, 1.f
    };
    // clang-format on

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  void PostProcessPass::onExit() {}

  void PostProcessPass::onResize(int width, int height) { postProcessFramebuffer_.resize(width, height); }

  void PostProcessPass::execute(const RenderContext& ctx) {
    auto* fovFramebuffer{ ctx.frameData.getFramebuffer("fov") };
    auto* postProcessMaterial{ ctx.materialManager.get(material_name::kPostProcess) };

    if (!fovFramebuffer || !postProcessMaterial)
      return;

    postProcessFramebuffer_.bind();
    render_system::clearColorBuffer();

    postProcessMaterial->bind();
    postProcessMaterial->applyProperties();

    postProcessMaterial->getShader().setInt("uScreenTexture", 0);
    fovFramebuffer->getColorTexture().bind(0);

    render_system::drawArrays(vao_, render_system::Primitive::Triangle, 0, 6);

    postProcessFramebuffer_.unbind();
    ctx.frameData.addFramebuffer("post_process", &postProcessFramebuffer_);
    ctx.frameData.addFramebuffer("final", &postProcessFramebuffer_);
  }

}  // namespace ls::renderer
