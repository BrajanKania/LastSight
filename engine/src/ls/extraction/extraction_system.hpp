#pragma once

#include "ls/ecs/registry.hpp"
#include "ls/ecs/types.hpp"
#include "ls/renderer/command_buffer.hpp"
#include "ls/renderer/commands/draw_sprite.hpp"

namespace ls::extraction_system {

  void extractRenderCommands(renderer::CommandBuffer& cmdBuffer, ecs::Registry& registry);

  renderer::command::DrawSprite extractDrawSpriteCommand(ecs::Registry& registry, ecs::EntityId entity);

}  // namespace ls::extraction_system
