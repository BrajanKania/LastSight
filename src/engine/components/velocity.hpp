#pragma once

#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Velocity {
    PROPERTY()
    glm::vec2 linear{ 0.f, 0.f };

    PROPERTY()
    float angular{ 0.f };
  };

}  // namespace ls::component
