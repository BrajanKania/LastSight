#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  enum class ColliderType { Circle, AABB, OBB };

  struct Collider {
    ColliderType type{ ColliderType::Circle };
    glm::vec2 halfExtents{ 1.f };
    float radius{ 1.f };
    glm::vec2 offset{ 0.f };
    bool isTrigger{ false };
  };

}  // namespace ls::component
