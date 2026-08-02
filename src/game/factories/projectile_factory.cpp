#include "game/factories/projectile_factory.hpp"

#include "engine/components/collider.hpp"
#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/ecs/types.hpp"
#include "engine/renderer/layer.hpp"
#include "game/components/projectile.hpp"

namespace ls::factory {

  void spawnProjectile(ecs::Registry& registry, const ProjectileConfig& config) {
    ecs::Entity proj{ registry.createEntity() };
    registry.addComponent(
        proj,
        component::Projectile{
            .lifetime = config.lifetime,
        }
    );

    float rotation{ glm::degrees(std::atan2(config.direction.y, config.direction.x)) };
    registry.addComponent(
        proj,
        component::Transform{
            .scale = config.scale,
            .position = config.position,
            .rotation = rotation,
        }
    );
    registry.addComponent(
        proj,
        component::Sprite{
            .color = glm::vec4(1.f),
            .uvScale = glm::vec2(1.f),
            .textureId = config.textureId,
            .angleOffset = config.angleOffset,
            .zIndex = layer::Entities,
        }
    );
    registry.addComponent(
        proj,
        component::Velocity{
            .linear = config.speed * config.direction,
        }
    );
    registry.addComponent(
        proj,
        component::Collider{
            .type = component::ColliderType::Circle,
            .radius = 0.5f,
        }
    );
  }

}  // namespace ls::factory
