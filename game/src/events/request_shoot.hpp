#pragma once

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct RequestShoot {
    ecs::EntityId shooter{ ecs::kNullEntity };
  };

}  // namespace ls::event
