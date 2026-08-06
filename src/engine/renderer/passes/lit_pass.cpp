#include "engine/renderer/passes/lit_pass.hpp"

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <utility>
#include <vector>

#include "engine/components/equipped_sprite.hpp"
#include "engine/components/particle_emitter.hpp"
#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/ecs/types.hpp"
#include "engine/gfx/framebuffer.hpp"
#include "engine/gfx/texture_2d.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/render_system.hpp"
#include "game/scenes/texture_names.hpp"

namespace ls::renderer {

  LitPass::LitPass(std::shared_ptr<gfx::Framebuffer> target)
      : IRenderPass{ std::move(target) } {}

  void LitPass::onEnter() {
    shader_.emplace(asset_system::shader("lit_vertex.glsl"), asset_system::shader("lit_fragment.glsl"));

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

  void LitPass::execute(const RenderContext& ctx) {
    targetFBO_->bind();

    render_system::setClearColor({ 0.2f, 0.2f, 0.2f, 1.f });
    render_system::clearColorBuffer();

    auto view{ ctx.registry.view<component::Transform, component::Sprite>() };
    std::vector<ecs::EntityId> renderQueue;
    renderQueue.reserve(view.size());

    for (auto entity : view) {
      renderQueue.push_back(entity);
    }

    std::stable_sort(renderQueue.begin(), renderQueue.end(), [&](ecs::EntityId a, ecs::EntityId b) {
      return ctx.registry.getComponent<component::Sprite>(a).zIndex <
             ctx.registry.getComponent<component::Sprite>(b).zIndex;
    });

    shader_->use();
    shader_->setMat4("uViewProjection", ctx.viewProjection);
    shader_->setInt("uTexture", 0);

    for (auto entity : renderQueue) {
      const auto& transform{ ctx.registry.getComponent<ls::component::Transform>(entity) };
      const auto& sprite{ ctx.registry.getComponent<ls::component::Sprite>(entity) };

      const gfx::Texture2D* texture{ ctx.textureManager.get(sprite.textureId) };
      if (texture) {
        texture->bind(0);
      }

      glm::mat4 model{ 1.f };
      model = glm::translate(model, glm::vec3(transform.position.x, transform.position.y, 0.f));
      model = glm::rotate(model, glm::radians(transform.rotation + sprite.angleOffset), glm::vec3(0.f, 0.f, 1.f));
      model = glm::scale(model, glm::vec3(transform.scale.x, transform.scale.y, 1.f));
      shader_->setMat4("uModel", model);
      shader_->setVec4("uColor", sprite.color);
      shader_->setVec2("uUvScale", sprite.uvScale);
      render_system::drawArrays(vao_, ls::render_system::Primitive::Triangle, 0, 6);
    }

    auto equippedView{ ctx.registry.view<component::Transform, component::EquippedSprite>() };
    for (auto entity : equippedView) {
      const auto& transform{ ctx.registry.getComponent<component::Transform>(entity) };
      const auto& equipped{ ctx.registry.getComponent<component::EquippedSprite>(entity) };

      const gfx::Texture2D* texture{ ctx.textureManager.get(equipped.textureId) };
      if (texture) {
        texture->bind(0);
      }

      glm::mat4 model{ 1.f };
      model = glm::translate(model, glm::vec3(transform.position.x, transform.position.y, 0.f));
      model = glm::rotate(model, glm::radians(transform.rotation + equipped.angleOffset), glm::vec3(0.f, 0.f, 1.f));
      model = glm::translate(model, glm::vec3(equipped.offset.x, equipped.offset.y, 0.f));
      glm::vec2 finalScale{ transform.scale * equipped.scale };
      model = glm::scale(model, glm::vec3(finalScale.x, finalScale.y, 1.f));

      shader_->setMat4("uModel", model);
      shader_->setVec4("uColor", glm::vec4(1.f));
      shader_->setVec2("uUvScale", glm::vec2(1.f));
      render_system::drawArrays(vao_, render_system::Primitive::Triangle, 0, 6);
    }

    shader_->setInt("uTexture", 0);
    const gfx::Texture2D* texture{ ctx.textureManager.get(texture_name::kWhite) };
    if (texture) {
      texture->bind(0);
    }
    shader_->setVec2("uUvScale", glm::vec2(1.f));

    auto particleView{ ctx.registry.view<component::ParticleEmitter>() };
    for (auto entity : particleView) {
      const auto& emitter{ ctx.registry.getComponent<component::ParticleEmitter>(entity) };

      for (std::size_t i = 0; i < emitter.activeParticlesCount; ++i) {
        const auto& particle{ emitter.particles[i] };

        glm::mat4 model{ 1.f };
        model = glm::translate(model, glm::vec3(particle.position.x, particle.position.y, 0.f));
        model = glm::rotate(model, glm::radians(particle.rotation), glm::vec3(0.f, 0.f, 1.f));
        model = glm::scale(model, glm::vec3(particle.scale.x, particle.scale.y, 1.f));

        shader_->setMat4("uModel", model);
        shader_->setVec4("uColor", particle.color);

        render_system::drawArrays(vao_, render_system::Primitive::Triangle, 0, 6);
      }
    }

    targetFBO_->unbind();
  }

}  // namespace ls::renderer
