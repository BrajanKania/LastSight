#include "game/systems/projectile_system.hpp"

#include <vector>

#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/collision.hpp"
#include "game/components/projectile.hpp"

namespace ls::projectile_system {

  void update(ecs::Registry& registry, dispatch::EventQueue& eventQueue, float dt) {
    std::vector<ecs::Entity> entitiesToDestroy{};

    for (ecs::Entity entity : registry.view<component::Projectile>()) {
      auto& projectile{ registry.getComponent<component::Projectile>(entity) };
      projectile.lifetime -= dt;
      if (projectile.lifetime <= 0.f) {
        entitiesToDestroy.push_back(entity);
      }
    }

    for (const auto& event : eventQueue.getEvents<event::Collision>()) {
      if (registry.hasComponent<component::Projectile>(event.entityA)) {
        entitiesToDestroy.push_back(event.entityA);
      }
      if (registry.hasComponent<component::Projectile>(event.entityB)) {
        entitiesToDestroy.push_back(event.entityB);
      }
    }

    for (ecs::Entity entity : entitiesToDestroy) {
      registry.destroyEntity(entity);
    }
  }

}  // namespace ls::projectile_system
