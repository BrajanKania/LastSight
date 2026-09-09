#pragma once

#include <glm/ext/matrix_float4x4.hpp>

#include "ls/gfx/texture_manager.hpp"
#include "ls/renderer/command_buffer.hpp"
#include "ls/renderer/frame_data.hpp"
#include "ls/renderer/material/material_manager.hpp"

namespace ls::renderer {

  struct RenderContext {
    const renderer::CommandBuffer& cmdBuffer;
    renderer::FrameData& frameData;
    renderer::MaterialManager& materialManager;
    gfx::TextureManager& textureManager;
  };

}  // namespace ls::renderer
