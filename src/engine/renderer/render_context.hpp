#pragma once

#include <glm/ext/matrix_float4x4.hpp>

#include "engine/gfx/texture_manager.hpp"
#include "engine/renderer/command_buffer.hpp"
#include "engine/renderer/frame_data.hpp"
#include "engine/renderer/material/material_manager.hpp"

namespace ls::renderer {

  struct RenderContext {
    const renderer::CommandBuffer& cmdBuffer;
    renderer::FrameData& frameData;
    renderer::MaterialManager& materialManager;
    gfx::TextureManager& textureManager;
  };

}  // namespace ls::renderer
