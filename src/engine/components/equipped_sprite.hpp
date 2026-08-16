#pragma once

#include <glm/glm.hpp>

#include "engine/gfx/texture_handle.hpp"
#include "engine/reflection/reflection_macros.hpp"
#include "engine/renderer/layer.hpp"

namespace ls::component {

  STRUCT()
  struct EquippedSprite {
    gfx::TextureHandle textureHandle{ 0 };
    glm::vec2 scale{ 1.f };
    glm::vec2 offset{ 0.f };
    float angleOffset{ -90.f };
    renderer::Layer layer{ renderer::Layer::Entities };
  };

}  // namespace ls::component
