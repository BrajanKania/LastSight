#include "factories/projectile_factory.hpp"

#include "components/projectile.hpp"
#include "ls/components/collider.hpp"
#include "ls/components/sprite.hpp"
#include "ls/components/transform.hpp"
#include "ls/components/velocity.hpp"
#include "ls/ecs/types.hpp"
#include "ls/renderer/layer.hpp"

namespace ls::factory {

  void spawnProjectile(ecs::Registry& registry, const ProjectileConfig& config) {
    ecs::EntityId proj{ registry.createEntity() };
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
            .materialHandle = config.materialHandle,
            .color = glm::vec4(1.f),
            .uvScale = glm::vec2(1.f),
            .textureHandle = config.textureHandle,
            .angleOffset = config.angleOffset,
            .layer = renderer::Layer::Entities,
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
