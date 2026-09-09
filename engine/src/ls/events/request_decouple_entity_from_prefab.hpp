#pragma once

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct RequestDecoupleEntityFromPrefab {
    ecs::EntityId entity{ ecs::kNullEntity };
  };

}  // namespace ls::event
