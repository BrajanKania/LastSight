#include "engine/renderer/passes/post_process_pass.hpp"

#include <glad/gl.h>

#include <glm/common.hpp>
#include <glm/ext/scalar_common.hpp>
#include <utility>

#include "engine/core/asset_system.hpp"
#include "engine/ecs/types.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/render_system.hpp"
#include "game/components/player.hpp"
#include "game/components/post_process_settings.hpp"

namespace ls::renderer {

  PostProcessPass::PostProcessPass(std::shared_ptr<gfx::Framebuffer> target, std::shared_ptr<gfx::Framebuffer> source)
      : IRenderPass{ std::move(target) },
        sourceFBO_{ source } {}

  void PostProcessPass::onEnter() {
    shader_.emplace(
        asset_system::shader("post_process_vertex.glsl"), asset_system::shader("post_process_fragment.glsl")
    );

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
    targetFBO_->bind();

    render_system::clearColorBuffer();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceFBO_->getColorBufferId());

    shader_->use();
    shader_->setInt("uTexture", 0);

    for (ecs::EntityId entity : ctx.registry.view<component::Player, component::PostProcessSettings>()) {
      const auto& settings{ ctx.registry.getComponent<component::PostProcessSettings>(entity) };

      shader_->setVec3("uDamageVignetteColor", settings.damageVignetteColor);
      shader_->setFloat("uDamageInnerRadius", settings.damageInnerRadius);
      shader_->setFloat("uDamageOuterRadius", settings.damageOuterRadius);
      shader_->setFloat("uMaxDamageDesaturation", settings.maxDamageDesaturation);
      shader_->setFloat("uDamageIntensity", settings.currentDamageIntensity);

      shader_->setFloat("uStaminaInnerRadius", settings.staminaInnerRadius);
      shader_->setFloat("uStaminaOuterRadius", settings.staminaOuterRadius);
      shader_->setFloat("uMaxStaminaDesaturation", settings.maxStaminaDesaturation);
      shader_->setFloat("uStaminaIntensity", settings.currentStaminaIntensity);

      render_system::drawArrays(vao_, render_system::Primitive::Triangle, 0, 6);
    }

    targetFBO_->unbind();
  }

}  // namespace ls::renderer
