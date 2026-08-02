#pragma once

#include <glm/glm.hpp>

#include "engine/renderer/layer.hpp"

namespace ls::component {

  struct Sprite {
    glm::vec4 color{ 1.f };
    glm::vec2 uvScale{ 1.f };
    uint32_t textureId{ 0 };
    float angleOffset{ 0.f };
    int zIndex{ layer::Ground };
  };

}  // namespace ls::component
