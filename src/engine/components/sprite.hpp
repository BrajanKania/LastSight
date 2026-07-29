#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  struct Sprite {
    glm::vec4 color{1.f};
    uint32_t textureId{0};
    int zIndex{0};
  };

}  // namespace ls::component
