#pragma once

#include "engine/ecs/types.hpp"

namespace ls::event {

  struct InteractWithEntity {
    ecs::EntityId interactor{ ecs::kNullEntity };
    ecs::EntityId target{ ecs::kNullEntity };
  };

}  // namespace ls::event
