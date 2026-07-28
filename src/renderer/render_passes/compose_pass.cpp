#include "compose_pass.hpp"

#include <glad/gl.h>

#include "core/asset_system.hpp"
#include "renderer/renderer_system.hpp"

namespace ls {

  ComposePass::ComposePass(std::shared_ptr<Framebuffer> litFramebuffer)
      : processedFBO_{litFramebuffer} {}

  void ComposePass::onEnter() {
    shader_.emplace(asset_system::shader("compose_vertex.glsl"), asset_system::shader("compose_fragment.glsl"));

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

  void ComposePass::execute(const RenderContext& ctx) {
    renderer_system::bindFramebuffer(0);

    renderer_system::clearColorBuffer();

    shader_->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, processedFBO_->getColorBufferID());
    shader_->setInt("uLitTexture", 0);

    renderer_system::drawArrays(vao_, renderer_system::Primitive::Triangle, 0, 6);
  }
}  // namespace ls
