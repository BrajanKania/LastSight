#pragma once

#include <glm/glm.hpp>

#include "ls/asset/asset_handle.hpp"
#include "ls/reflection/reflection_macros.hpp"
#include "ls/renderer/layer.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::component {

  COMPONENT(DisplayName("Equipped Sprite"))
  struct EquippedSprite {
    PROPERTY(DisplayName("Texture"))
    asset::TextureHandle textureHandle{ asset::kNullAsset };

    PROPERTY(DisplayName("Material"))
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };

    PROPERTY(DisplayName("Scale"))
    glm::vec2 scale{ 1.f };

    PROPERTY(DisplayName("Offset"))
    glm::vec2 offset{ 0.f };

    PROPERTY(DisplayName("Angle Offset"), Step(0.5f))
    float angleOffset{ -90.f };

    PROPERTY(DisplayName("Layer"))
    renderer::Layer layer{ renderer::Layer::Entities };
  };

}  // namespace ls::component
