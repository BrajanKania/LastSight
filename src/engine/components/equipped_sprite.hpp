#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "engine/renderer/layer.hpp"

namespace ls::component {

  struct EquippedSprite {
    uint32_t textureId{ 0 };
    glm::vec2 scale{ 1.f };
    glm::vec2 offset{ 0.f };
    float angleOffset{ -90.f };
    renderer::Layer zIndex{ renderer::Layer::Entities };
  };

}  // namespace ls::component
