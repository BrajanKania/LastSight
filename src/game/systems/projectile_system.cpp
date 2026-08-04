#include "game/systems/projectile_system.hpp"

#include "engine/core/update_context.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/collision.hpp"
#include "game/components/projectile.hpp"

namespace ls::projectile_system {

  void update(const UpdateContext& ctx) {
    for (ecs::EntityId entity : ctx.registry.view<component::Projectile>()) {
      auto& projectile{ ctx.registry.getComponent<component::Projectile>(entity) };
      projectile.lifetime -= ctx.dt;
      if (projectile.lifetime <= 0.f) {
        ctx.registry.destroyEntity(entity);
      }
    }

    for (const auto& event : ctx.eventQueue.getEvents<event::Collision>()) {
      if (ctx.registry.hasComponent<component::Projectile>(event.entityA)) {
        ctx.registry.destroyEntity(event.entityA);
      }
      if (ctx.registry.hasComponent<component::Projectile>(event.entityB)) {
        ctx.registry.destroyEntity(event.entityB);
      }
    }
  }

}  // namespace ls::projectile_system
