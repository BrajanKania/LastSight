#pragma once

#include <glm/glm.hpp>

#include "engine/gfx/texture_handle.hpp"
#include "engine/reflection/reflection_macros.hpp"
#include "engine/renderer/layer.hpp"

namespace ls::component {

  STRUCT()
  struct Sprite {
    glm::vec4 color{ 1.f };
    glm::vec2 uvScale{ 1.f };
    gfx::TextureHandle textureHandle{ 0 };
    float angleOffset{ 0.f };
    renderer::Layer layer{ renderer::Layer::Ground };
  };

}  // namespace ls::component
