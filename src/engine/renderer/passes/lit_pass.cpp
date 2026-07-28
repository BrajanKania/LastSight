#include "engine/renderer/passes/lit_pass.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <memory>

#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/renderer_system.hpp"

namespace ls {

  LitPass::LitPass(std::shared_ptr<Framebuffer> framebuffer)
      : worldFBO_{std::move(framebuffer)} {}

  void LitPass::onEnter() {
    shader_.emplace(asset_system::shader("lit_vertex.glsl"), asset_system::shader("lit_fragment.glsl"));

    // clang-format off
    float vertex[] {
      -0.5f, -0.5f,
      0.5f, -0.5f,
      0.5f, 0.5f,

      -0.5f, -0.5f,
      0.5f, 0.5f,
      -0.5f, 0.5f
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

  void LitPass::execute(const RenderContext& ctx) {
    worldFBO_->bind();

    renderer_system::setClearColor({0.2f, 0.2f, 0.2f, 1.f});
    renderer_system::clearColorBuffer();

    shader_->use();

    for (auto entity : ctx.registry.view<component::Transform, component::Sprite>()) {
      const auto& transform{ctx.registry.getComponent<ls::component::Transform>(entity)};
      const auto& sprite{ctx.registry.getComponent<ls::component::Sprite>(entity)};

      glm::mat4 model{1.f};
      model = glm::translate(model, transform.position);
      model = glm::scale(model, transform.scale);
      shader_->setMat4("uModel", model);
      shader_->setVec3("uColor", sprite.color);
      renderer_system::drawArrays(vao_, ls::renderer_system::Primitive::Triangle, 0, 6);
    }

    worldFBO_->unBind();
  }

}  // namespace ls
