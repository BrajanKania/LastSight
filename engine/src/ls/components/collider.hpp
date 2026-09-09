#pragma once

#include <glm/glm.hpp>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  ENUM()
  enum class ColliderType : int { Circle, AABB, OBB };

  COMPONENT()
  struct Collider {
    PROPERTY()
    ColliderType type{ ColliderType::Circle };

    PROPERTY()
    glm::vec2 halfExtents{ 1.f };

    PROPERTY()
    float radius{ 1.f };

    PROPERTY()
    glm::vec2 offset{ 0.f };

    PROPERTY()
    bool isTrigger{ false };
  };

}  // namespace ls::component
