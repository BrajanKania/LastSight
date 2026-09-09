#pragma once

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct RequestInteraction {
    ecs::EntityId interactor{ ecs::kNullEntity };
  };

}  // namespace ls::event
