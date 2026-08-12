#pragma once

#include <glm/ext/vector_float2.hpp>

namespace ls::component {

  struct Parallax {
    glm::vec2 factor{ 0.f };
    glm::vec2 basePosition{ 0.f };
  };

}  // namespace ls::component
