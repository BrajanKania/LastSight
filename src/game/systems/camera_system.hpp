#pragma once

#include "engine/core/update_context.hpp"
#include "engine/ecs/types.hpp"

namespace ls::camera_system {

  void update(const UpdateContext& ctx);

  void follow(const UpdateContext& ctx, const ecs::EntityId targetEntity, float smoothness);

}  // namespace ls::camera_system
