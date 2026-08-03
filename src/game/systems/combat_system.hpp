#pragma once

#include "engine/core/update_context.hpp"
#include "engine/ecs/types.hpp"

namespace ls::combat_system {

  void update(const UpdateContext& ctx);

  void shoot(const UpdateContext& ctx, const ecs::EntityId shooterEntity);

}  // namespace ls::combat_system
