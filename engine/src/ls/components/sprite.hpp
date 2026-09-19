#pragma once

#include <glm/glm.hpp>

#include "ls/asset/asset_handle.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/reflection/reflection_macros.hpp"
#include "ls/renderer/layer.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::component {

  COMPONENT(DisplayName("Sprite"))
  struct Sprite {
    PROPERTY(DisplayName("Texture"))
    asset::TextureHandle textureHandle{ asset::kNullAsset };

    PROPERTY(DisplayName("Material"))
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };

    PROPERTY(DisplayName("Color"), Color)
    glm::vec4 color{ 1.f };

    PROPERTY(DisplayName("UV Scale"))
    glm::vec2 uvScale{ 1.f };

    PROPERTY(DisplayName("Angle Offset"), Step(0.5f))
    float angleOffset{ 0.f };

    PROPERTY(DisplayName("Layer"))
    renderer::Layer layer{ renderer::Layer::Ground };
  };

}  // namespace ls::component
