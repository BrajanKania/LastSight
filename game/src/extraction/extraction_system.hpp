#pragma once

#include "ls/ecs/registry.hpp"
#include "ls/renderer/command_buffer.hpp"

namespace ls::extraction_system {

  void extractGameRenderCommands(renderer::CommandBuffer& cmdBuffer, ecs::Registry& registry);

}
