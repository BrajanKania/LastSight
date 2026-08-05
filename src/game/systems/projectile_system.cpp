#include "game/systems/projectile_system.hpp"

#include "engine/components/particle_emitter.hpp"
#include "engine/components/transform.hpp"
#include "engine/core/update_context.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/collision.hpp"
#include "game/components/enemy.hpp"
#include "game/components/projectile.hpp"
#include "game/particles/blood_splat.hpp"

namespace ls::projectile_system {
  namespace {

    void hitEnemy(ecs::Registry& registry, const ecs::EntityId enemy) {
      auto particleEmitter{ registry.createEntity() };
      registry.addComponent(particleEmitter, component::ParticleEmitter{ particle::preset::bloodSplat() });
      registry.addComponent(
          particleEmitter,
          component::Transform{ .position = registry.getComponent<component::Transform>(enemy).position }
      );
    }

  }  // namespace

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
        if (ctx.registry.hasComponent<component::Enemy>(event.entityB)) {
          hitEnemy(ctx.registry, event.entityB);
        }
      }
      if (ctx.registry.hasComponent<component::Projectile>(event.entityB)) {
        ctx.registry.destroyEntity(event.entityB);
        if (ctx.registry.hasComponent<component::Enemy>(event.entityA)) {
          hitEnemy(ctx.registry, event.entityA);
        }
      }
    }
  }

}  // namespace ls::projectile_system
