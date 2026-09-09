#pragma once

#include <glm/ext/vector_float2.hpp>

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct MenuStalkerPoint {
    PROPERTY()
    glm::vec2 position{ 0.f };

    PROPERTY()
    glm::vec2 peekOffset{ -0.2f, 0.f };

    PROPERTY()
    glm::vec2 parallaxFactor{ 0.f };
  };

}  // namespace ls::component
