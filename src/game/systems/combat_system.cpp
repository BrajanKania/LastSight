#include "game/systems/combat_system.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/trigonometric.hpp>

#include "engine/components/particle_emitter.hpp"
#include "engine/components/transform.hpp"
#include "engine/core/update_context.hpp"
#include "engine/ecs/types.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "game/components/weapon.hpp"
#include "game/factories/projectile_factory.hpp"
#include "game/particles/gun_sparks.hpp"

namespace ls::combat_system {

  void update(const UpdateContext& ctx) {
    for (auto entity : ctx.registry.view<component::Weapon>()) {
      auto& weapon{ ctx.registry.getComponent<component::Weapon>(entity) };
      if (weapon.cooldown > 0.f) {
        weapon.cooldown -= ctx.dt;
      }
    }
  }

  void shoot(const UpdateContext& ctx, const ecs::EntityId shooterEntity) {
    const auto& transform{ ctx.registry.getComponent<component::Transform>(shooterEntity) };
    auto& weapon{ ctx.registry.getComponent<component::Weapon>(shooterEntity) };

    float angleRad{ glm::radians(transform.rotation) };
    glm::vec2 forward{ glm::vec2(glm::cos(angleRad), glm::sin(angleRad)) };
    glm::vec2 right{ -forward.y, forward.x };
    glm::vec2 rotatedOffset{ forward * weapon.barrelOffset.y + right * weapon.barrelOffset.x };

    factory::ProjectileConfig config{
      .scale = weapon.bulletScale,
      .position = transform.position + rotatedOffset,
      .direction = forward,
      .speed = weapon.initialSpeed,
      .textureId = ctx.textureManager.getId("bullet"),
      .angleOffset = -90.f,
      .lifetime = weapon.bulletLifetime,
    };

    factory::spawnProjectile(ctx.registry, config);

    ecs::EntityId particleEmitter{ ctx.registry.createEntity() };
    ctx.registry.addComponent(particleEmitter, component::ParticleEmitter{ particle::preset::gunSparks() });
    ctx.registry.addComponent(
        particleEmitter,
        component::Transform{
            .position = transform.position + rotatedOffset,
        }
    );

    weapon.cooldown = weapon.fireRate;
  }

}  // namespace ls::combat_system
