#pragma once

#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  ENUM()
  enum class ColliderType : int { Circle, AABB, OBB };

  STRUCT()
  struct Collider {
    ColliderType type{ ColliderType::Circle };
    glm::vec2 halfExtents{ 1.f };
    float radius{ 1.f };
    glm::vec2 offset{ 0.f };
    bool isTrigger{ false };
  };

}  // namespace ls::component
