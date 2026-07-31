#include "engine/renderer/passes/fov_pass.hpp"

#include <glad/gl.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <memory>

#include "engine/components/transform.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/renderer_system.hpp"
#include "game/components/field_of_view.hpp"
#include "game/components/player.hpp"

namespace ls {

  FovPass::FovPass(std::shared_ptr<Framebuffer> target, std::shared_ptr<Framebuffer> source)
      : targetFBO_{ std::move(target) },
        sourceFBO_{ std::move(source) } {}

  void FovPass::onEnter() {
    shader_.emplace(asset_system::shader("fov_vertex.glsl"), asset_system::shader("fov_fragment.glsl"));

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

  void FovPass::execute(const RenderContext& ctx) {
    targetFBO_->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceFBO_->getColorBufferID());

    shader_->use();
    shader_->setInt("uScreenTexture", 0);

    for (auto entity : ctx.registry.view<component::Player, component::Transform, component::FieldOfView>()) {
      const auto& fieldOfView{ ctx.registry.getComponent<component::FieldOfView>(entity) };
      const auto& transform{ ctx.registry.getComponent<component::Transform>(entity) };

      float rad{ glm::radians(transform.rotation + 90.0f) };
      glm::vec2 playerDir{ std::cos(rad), std::sin(rad) };

      shader_->setVec2("uViewPos", transform.position);
      shader_->setVec2("uViewDir", playerDir);
      shader_->setMat4("uInvViewProj", glm::inverse(ctx.viewProjection));

      shader_->setFloat("uInnerRadius", fieldOfView.innerRadius);
      shader_->setFloat("uOuterRadius", fieldOfView.outerRadius);
      shader_->setFloat("uHalfFovRad", glm::radians(fieldOfView.fovAngle / 2.f));
      shader_->setFloat("uSmoothnessRad", glm::radians(fieldOfView.smoothnessAngle));
      shader_->setFloat("uSmoothnessDistance", fieldOfView.smoothnessDistance);

      renderer_system::drawArrays(vao_, renderer_system::Primitive::Triangle, 0, 6);
      break;
    }

    targetFBO_->unBind();
  }

}  // namespace ls
