#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Parallax {
    PROPERTY(DisplayName("Factor"))
    glm::vec2 factor{ 0.f };

    PROPERTY(DisplayName("Base Position"))
    glm::vec2 basePosition{ 0.f };
  };

}  // namespace ls::component
