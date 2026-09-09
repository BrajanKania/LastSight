#pragma once

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct EntityDuplicated {
    ecs::EntityId sourceEntity{ ecs::kNullEntity };
    ecs::EntityId newEntity{ ecs::kNullEntity };
  };

}  // namespace ls::event
