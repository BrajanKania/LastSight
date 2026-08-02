#pragma once

#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"

namespace ls::camera_system {

  void update(ecs::Registry& registry);

  void follow(ecs::Registry& registry, const ecs::EntityId targetEntity, float dt, float smoothness);

}  // namespace ls::camera_system
