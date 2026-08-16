#pragma once

#include "engine/ecs/types.hpp"
namespace ls::event {

  struct RequestSaveEntity {
    ecs::EntityId entity{ ecs::kNullEntity };
  };

}  // namespace ls::event
