#pragma once

#include <glm/glm.hpp>

#include "ls/gfx/texture_handle.hpp"
#include "ls/reflection/reflection_macros.hpp"
#include "ls/renderer/layer.hpp"
#include "ls/renderer/material/material_handle.hpp"

namespace ls::component {

  COMPONENT()
  struct Sprite {
    PROPERTY(DisplayName("Material Handle"))
    renderer::MaterialHandle materialHandle{ renderer::kNullMaterial };

    PROPERTY(DisplayName("Color"), Color)
    glm::vec4 color{ 1.f };

    PROPERTY(DisplayName("UV Scale"))
    glm::vec2 uvScale{ 1.f };

    PROPERTY(DisplayName("Texture"))
    gfx::TextureHandle textureHandle{ 0 };

    PROPERTY(DisplayName("Angle Offset"), Step(0.5f))
    float angleOffset{ 0.f };

    PROPERTY(DisplayName("Layer"))
    renderer::Layer layer{ renderer::Layer::Ground };
  };

}  // namespace ls::component
