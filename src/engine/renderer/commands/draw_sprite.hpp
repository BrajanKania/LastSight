#pragma once

#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>

#include "engine/gfx/texture_handle.hpp"
#include "engine/renderer/layer.hpp"
#include "engine/renderer/material/material_handle.hpp"
#include "engine/renderer/material/material_property.hpp"

namespace ls::renderer::command {

  struct DrawSprite {
    MaterialHandle materialHandle{ renderer::kNullMaterial };
    gfx::TextureHandle textureHandle{};
    Layer layer{ Layer::Ground };
    glm::vec4 color{ 1.f };
    glm::vec2 uvScale{ 1.f };
    glm::mat4 model{ 1.f };
    std::array<MaterialProperty, 4> custom{};
  };

}  // namespace ls::renderer::command
