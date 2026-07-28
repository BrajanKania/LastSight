#include "game/systems/player_system.hpp"

#include <glm/geometric.hpp>
#include <glm/glm.hpp>

#include "engine/components/velocity.hpp"
#include "engine/core/input_system.hpp"
#include "engine/ecs/registry.hpp"
#include "game/components/movement.hpp"
#include "game/components/player.hpp"

namespace ls::player_system {

  namespace {

    void handleInput(ecs::Registry& registry, const ecs::Entity playerEntity) {
      auto& velocity{registry.getComponent<component::Velocity>(playerEntity)};
      const auto& movement{registry.getComponent<component::Movement>(playerEntity)};

      float speed{(input_system::isKeyPressed(input_system::Key::LShift) ? movement.runSpeed : movement.walkSpeed)};

      glm::vec2 direction{0.f, 0.f};

      if (input_system::isKeyPressed(input_system::Key::W))
        direction.y += 1.f;

      if (input_system::isKeyPressed(input_system::Key::A))
        direction.x -= 1.f;

      if (input_system::isKeyPressed(input_system::Key::S))
        direction.y -= 1.f;

      if (input_system::isKeyPressed(input_system::Key::D))
        direction.x += 1.f;

      velocity.linear = (glm::length(direction) != 0.f ? glm::normalize(direction) * speed : glm::vec2(0.f));
    }

  }  // namespace

  void update(ecs::Registry& registry, float dt) {
    for (auto entity : registry.view<component::Player, component::Velocity, component::Movement>()) {
      handleInput(registry, entity);
    }
  }

}  // namespace ls::player_system
