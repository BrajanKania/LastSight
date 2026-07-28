#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  struct Transform {
    glm::vec3 scale{};
    glm::vec3 position{};
  };

}  // namespace ls::component
