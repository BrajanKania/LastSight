#include "post_process_pass.hpp"

#include <glad/gl.h>

#include "core/asset_system.hpp"
#include "renderer/renderer_system.hpp"

namespace ls {

  PostProcessPass::PostProcessPass(std::shared_ptr<Framebuffer> processedFBO, std::shared_ptr<Framebuffer> worldFBO)
      : processedFBO_{processedFBO},
        worldFBO_{worldFBO} {}

  void PostProcessPass::onEnter() {
    shader_.emplace(asset_system::shader("post_process_vertex.glsl"),
                    asset_system::shader("post_process_fragment.glsl"));

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

  void PostProcessPass::execute(const RenderContext& ctx) {
    processedFBO_->bind();

    renderer_system::clearColorBuffer();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, worldFBO_->getColorBufferID());

    shader_->use();
    shader_->setInt("uTexture", 0);

    renderer_system::drawArrays(vao_, renderer_system::Primitive::Triangle, 0, 6);

    processedFBO_->unBind();
  }
}  // namespace ls
