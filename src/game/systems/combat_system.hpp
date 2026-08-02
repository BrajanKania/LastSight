#pragma once

#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/renderer/texture_manager.hpp"

namespace ls::combat_system {

  void update(ecs::Registry& registry, float dt);

  void shoot(ecs::Registry& registry, const ecs::Entity shooterEntity, const TextureManager& textureManager);

}  // namespace ls::combat_system
