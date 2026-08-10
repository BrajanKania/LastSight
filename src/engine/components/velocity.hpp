#pragma once

#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Velocity {
    glm::vec2 linear{ 0.f, 0.f };
    float angular{ 0.f };
  };

}  // namespace ls::component
