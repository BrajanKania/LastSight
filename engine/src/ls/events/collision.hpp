#pragma once

#include <glm/glm.hpp>

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct Collision {
    ecs::EntityId entityA;
    ecs::EntityId entityB;
    bool isTrigger{ false };
    glm::vec2 normal{ 0.f, 1.f };
    float depth{ 0.f };
  };

}  // namespace ls::event
