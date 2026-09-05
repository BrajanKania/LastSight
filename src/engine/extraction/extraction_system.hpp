#pragma once

#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/renderer/command_buffer.hpp"
#include "engine/renderer/commands/draw_sprite.hpp"

namespace ls::extraction_system {

  void extractRenderCommands(renderer::CommandBuffer& cmdBuffer, ecs::Registry& registry);

  renderer::command::DrawSprite extractDrawSpriteCommand(ecs::Registry& registry, ecs::EntityId entity);

}  // namespace ls::extraction_system
