#include "game/systems/combat_system.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/trigonometric.hpp>

#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/renderer/texture_manager.hpp"
#include "game/components/weapon.hpp"
#include "game/factories/projectile_factory.hpp"

namespace ls::combat_system {

  void update(ecs::Registry& registry, float dt) {
    for (auto entity : registry.view<component::Weapon>()) {
      auto& weapon{ registry.getComponent<component::Weapon>(entity) };
      if (weapon.cooldown > 0.f) {
        weapon.cooldown -= dt;
      }
    }
  }

  void shoot(ecs::Registry& registry, const ecs::Entity shooterEntity, const TextureManager& textureManager) {
    const auto& transform{ registry.getComponent<component::Transform>(shooterEntity) };
    const auto& sprite{ registry.getComponent<component::Sprite>(shooterEntity) };
    auto& weapon{ registry.getComponent<component::Weapon>(shooterEntity) };

    float angleRad{ glm::radians(transform.rotation) };
    glm::vec2 forward{ glm::vec2(glm::cos(angleRad), glm::sin(angleRad)) };
    glm::vec2 right{ -forward.y, forward.x };
    glm::vec2 rotatedOffset{ forward * weapon.barrelOffset.y + right * weapon.barrelOffset.x };

    factory::ProjectileConfig config{
      .scale = weapon.bulletScale,
      .position = transform.position + rotatedOffset,
      .direction = forward,
      .speed = weapon.initialSpeed,
      .textureId = textureManager.getId("bullet"),
      .angleOffset = -90.f,
      .lifetime = weapon.bulletLifetime,
    };

    factory::spawnProjectile(registry, config);

    weapon.cooldown = weapon.fireRate;
  }

}  // namespace ls::combat_system
