#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  struct Transform {
    glm::vec2 scale{1.f};
    glm::vec2 position{0.f};
    float rotation{0.f};
  };

}  // namespace ls::component
