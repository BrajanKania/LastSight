#pragma once

#include "engine/ecs/registry.hpp"
#include "engine/renderer/texture_manager.hpp"

namespace ls::player_system {

  void update(ecs::Registry& registry, const TextureManager& textureManager, float dt);

}
