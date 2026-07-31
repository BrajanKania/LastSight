#include "game/systems/player_system.hpp"

#include <unistd.h>

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>

#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/core/input_system.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/renderer/renderer_system.hpp"
#include "game/components/camera.hpp"
#include "game/components/movement.hpp"
#include "game/components/player.hpp"

namespace ls::player_system {

  namespace {

    void handleInput(ecs::Registry& registry, const ecs::Entity playerEntity) {
      auto& velocity{ registry.getComponent<component::Velocity>(playerEntity) };
      const auto& movement{ registry.getComponent<component::Movement>(playerEntity) };

      float speed{ (input_system::isKeyPressed(input_system::Key::LShift) ? movement.runSpeed : movement.walkSpeed) };

      glm::vec2 direction{ 0.f, 0.f };

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

    glm::vec2 screenToWorld(glm::vec2 mousePos, glm::vec2 viewportSize, const glm::mat4& invViewProjection) {
      float ndcX{ (2.0f * mousePos.x) / viewportSize.x - 1.0f };
      float ndcY{ 1.0f - (2.0f * mousePos.y) / viewportSize.y };

      glm::vec4 worldPos{ invViewProjection * glm::vec4(ndcX, ndcY, 0.0f, 1.0f) };
      return glm::vec2(worldPos.x, worldPos.y);
    }

    void handleRotation(ecs::Registry& registry, const ecs::Entity playerEntity, float dt) {
      const auto& movement{ registry.getComponent<component::Movement>(playerEntity) };
      const auto& transform{ registry.getComponent<component::Transform>(playerEntity) };
      auto& velocity{ registry.getComponent<component::Velocity>(playerEntity) };

      for (auto entity : registry.view<component::Camera>()) {
        const auto& camera{ registry.getComponent<component::Camera>(entity) };

        glm::mat4 invViewProjection{ glm::inverse(camera.projection * camera.view) };
        glm::vec2 viewportSize{ renderer_system::getViewportSize() };
        glm::vec2 mouseScreenPos{ input_system::getMousePosition() };

        glm::vec2 mouseWorldPos{ screenToWorld(mouseScreenPos, viewportSize, invViewProjection) };

        glm::vec2 direction{ mouseWorldPos - transform.position };
        if (glm::length(direction) > 0.001f) {
          float targetAngle{ glm::degrees(std::atan2(direction.y, direction.x)) - 90.f };
          float angleDiff{ std::remainder(targetAngle - transform.rotation, 360.f) };
          float desiredAngularSpeed{ angleDiff / dt };
          float maxAngularSpeed{ movement.angularSpeed };
          velocity.angular = glm::clamp(desiredAngularSpeed, -maxAngularSpeed, maxAngularSpeed);
        } else {
          velocity.angular = 0.f;
        }
        break;
      }
    }

  }  // namespace

  void update(ecs::Registry& registry, float dt) {
    for (auto entity : registry.view<component::Player, component::Velocity, component::Movement>()) {
      handleInput(registry, entity);
      handleRotation(registry, entity, dt);
    }
  }

}  // namespace ls::player_system
