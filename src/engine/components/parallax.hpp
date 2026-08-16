#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Parallax {
    glm::vec2 factor{ 0.f };
    glm::vec2 basePosition{ 0.f };
  };

}  // namespace ls::component
