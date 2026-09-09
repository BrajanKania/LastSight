#pragma once

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct RequestDestroyEntity {
    ecs::EntityId entity{ ecs::kNullEntity };
  };

}  // namespace ls::event
