#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  struct Transform {
    glm::vec3 scale{};
    glm::vec3 position{};
  };

  struct Sprite {
    glm::vec3 color{};
  };

}  // namespace ls::component
