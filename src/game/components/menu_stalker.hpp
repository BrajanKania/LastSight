#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct MenuStalker {
    glm::vec2 hidePosition{ 0.f };
    glm::vec2 peekOffset{ 0.2f, 0.f };
    float minDistance{ 1.f };
    float maxDistance{ 2.f };
    float peekSpeed{ 0.1f };

    PROPERTY(ReadOnly, Transient)
    float currentPeek{ 0.f };
  };

}  // namespace ls::component
