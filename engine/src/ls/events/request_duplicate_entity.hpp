#pragma once

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct RequestDuplicateEntity {
    const ecs::EntityId entityToDuplicate{ ecs::kNullEntity };
  };

}  // namespace ls::event
