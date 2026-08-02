#include "game/systems/projectile_system.hpp"

#include <vector>

#include "engine/ecs/types.hpp"
#include "game/components/projectile.hpp"

namespace ls::projectile_system {

  void update(ecs::Registry& registry, float dt) {
    std::vector<ecs::Entity> entitiesToDestroy{};

    for (ecs::Entity entity : registry.view<component::Projectile>()) {
      auto& projectile{ registry.getComponent<component::Projectile>(entity) };
      projectile.lifetime -= dt;
      if (projectile.lifetime <= 0.f) {
        entitiesToDestroy.push_back(entity);
      }
    }

    for (ecs::Entity entity : entitiesToDestroy) {
      registry.destroyEntity(entity);
    }
  }

}  // namespace ls::projectile_system
