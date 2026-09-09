#include "renderer/passes/fov_pass.hpp"

#include <glad/gl.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>

#include "ls/renderer/material/material_names.hpp"
#include "ls/renderer/render_system.hpp"

namespace ls::renderer {

  void FovPass::onEnter() {
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

  void FovPass::onExit() {}

  void FovPass::onResize(int width, int height) { fovFramebuffer_.resize(width, height); }

  void FovPass::execute(const RenderContext& ctx) {
    auto* litFramebuffer{ ctx.frameData.getFramebuffer("lit") };
    auto* fovMaterial{ ctx.materialManager.get(material_name::kFov) };

    if (!litFramebuffer || !fovMaterial)
      return;

    fovFramebuffer_.bind();
    render_system::clearColorBuffer();

    fovMaterial->bind();
    fovMaterial->applyProperties();

    fovMaterial->getShader().setInt("uScreenTexture", 0);
    litFramebuffer->getColorTexture().bind(0);

    fovMaterial->getShader().setMat4(
        "uInvViewProj", glm::inverse(ctx.frameData.getProjection() * ctx.frameData.getView())
    );

    render_system::drawArrays(vao_, render_system::Primitive::Triangle, 0, 6);

    fovFramebuffer_.unbind();
    ctx.frameData.addFramebuffer("fov", &fovFramebuffer_);
    ctx.frameData.addFramebuffer("final", &fovFramebuffer_);
  }

}  // namespace ls::renderer
