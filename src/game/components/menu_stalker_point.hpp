#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct MenuStalkerPoint {
    glm::vec2 position{ 0.f };
    glm::vec2 peekOffset{ -0.2f, 0.f };
    glm::vec2 parallaxFactor{ 0.f };
  };

}  // namespace ls::component
