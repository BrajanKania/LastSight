#pragma once

#include "engine/ecs/registry.hpp"
#include "engine/renderer/command_buffer.hpp"

namespace ls::extraction_system {

  void extractGameRenderCommands(renderer::CommandBuffer& cmdBuffer, ecs::Registry& registry);

}
