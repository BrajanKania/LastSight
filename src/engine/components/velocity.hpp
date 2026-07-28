#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  struct Velocity {
    glm::vec2 linear{0.f, 0.f};
  };

}  // namespace ls::component
