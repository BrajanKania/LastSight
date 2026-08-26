#pragma once

#include <glm/glm.hpp>

#include "engine/gfx/texture_handle.hpp"
#include "engine/reflection/reflection_macros.hpp"
#include "engine/renderer/layer.hpp"

namespace ls::component {

  COMPONENT()
  struct EquippedSprite {
    PROPERTY()
    gfx::TextureHandle textureHandle{ 0 };

    PROPERTY()
    glm::vec2 scale{ 1.f };

    PROPERTY()
    glm::vec2 offset{ 0.f };

    PROPERTY()
    float angleOffset{ -90.f };

    PROPERTY()
    renderer::Layer layer{ renderer::Layer::Entities };
  };

}  // namespace ls::component
