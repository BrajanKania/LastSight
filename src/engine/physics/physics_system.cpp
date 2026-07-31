#include "engine/physics/physics_system.hpp"

#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/ecs/registry.hpp"

namespace ls::physics_system {

  namespace {

    void applyVelocity(ecs::Registry& registry, float dt) {
      for (auto entity : registry.view<component::Transform, component::Velocity>()) {
        auto& transform{ registry.getComponent<component::Transform>(entity) };
        const auto& velocity{ registry.getComponent<component::Velocity>(entity) };

        transform.position += velocity.linear * dt;
        transform.rotation += velocity.angular * dt;
      }
    }

  }  // namespace

  void update(ecs::Registry& registry, float dt) { applyVelocity(registry, dt); }

}  // namespace ls::physics_system
