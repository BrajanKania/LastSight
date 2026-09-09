#include "extraction/extraction_system.hpp"

#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>

#include "components/fov_masked.hpp"
#include "ls/components/equipped_sprite.hpp"
#include "ls/components/particle_emitter.hpp"
#include "ls/components/sprite.hpp"
#include "ls/components/transform.hpp"
#include "ls/extraction/extraction_system.hpp"
#include "ls/renderer/commands/draw_sprite.hpp"
#include "ls/renderer/material/material_property.hpp"

namespace ls::extraction_system {

  void extractGameRenderCommands(renderer::CommandBuffer& cmdBuffer, ecs::Registry& registry) {
    for (auto entity : registry.view<component::Transform, component::Sprite>()) {
      auto cmd{ extractDrawSpriteCommand(registry, entity) };
      if (registry.hasComponent<component::FovMasked>(entity)) {
        cmd.custom[0] = renderer::MaterialProperty{
          .name = "uRequiresFov",
          .value = true,
        };
      };

      cmdBuffer.push(cmd);
    }

    for (auto entity : registry.view<component::EquippedSprite, component::Transform>()) {
      const auto& equippedSprite{ registry.getComponent<component::EquippedSprite>(entity) };
      const auto& transform{ registry.getComponent<component::Transform>(entity) };

      glm::mat4 model{ 1.f };
      model = glm::translate(model, glm::vec3{ transform.position.x, transform.position.y, 0.f });
      model =
          glm::rotate(model, glm::radians(transform.rotation + equippedSprite.angleOffset), glm::vec3{ 0.f, 0.f, 1.f });
      model = glm::translate(model, glm::vec3{ equippedSprite.offset.x, equippedSprite.offset.y, 0.f });
      model = glm::scale(
          model,
          glm::vec3{ transform.scale.x * equippedSprite.scale.x, transform.scale.y * equippedSprite.scale.y, 1.f }
      );

      auto cmd{ renderer::command::DrawSprite{
          .materialHandle = equippedSprite.materialHandle,
          .textureHandle = equippedSprite.textureHandle,
          .layer = equippedSprite.layer,
          .model = model,
      } };

      if (registry.hasComponent<component::FovMasked>(entity)) {
        cmd.custom[0] = renderer::MaterialProperty{
          .name = "uRequiresFov",
          .value = true,
        };
      }

      cmdBuffer.push(cmd);
    }

    for (auto entity : registry.view<component::ParticleEmitter>()) {
      const auto& emitter{ registry.getComponent<component::ParticleEmitter>(entity) };

      for (std::size_t i{ 0 }; i < emitter.activeParticlesCount; i++) {
        const auto& particle{ emitter.particles[i] };
        glm::mat4 model{ 1.f };
        model = glm::translate(model, glm::vec3(particle.position.x, particle.position.y, 0.f));
        model = glm::rotate(model, glm::radians(particle.rotation), glm::vec3(0.f, 0.f, 1.f));
        model = glm::scale(model, glm::vec3(particle.scale.x, particle.scale.y, 1.f));

        auto cmd{ renderer::command::DrawSprite{
            .materialHandle = emitter.config.materialHandle,
            .textureHandle = emitter.config.textureHandle,
            .layer = emitter.config.layer,
            .color = particle.color,
            .model = model,
        } };

        if (registry.hasComponent<component::FovMasked>(entity)) {
          cmd.custom[0] = renderer::MaterialProperty{
            .name = "uRequiresFov",
            .value = true,
          };
        }

        cmdBuffer.push(cmd);
      }
    }
  }

}  // namespace ls::extraction_system
