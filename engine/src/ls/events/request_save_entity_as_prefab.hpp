#pragma once

#include <string>

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct RequestSaveEntityAsPrefab {
    std::string name{};
    ecs::EntityId entity{ ecs::kNullEntity };
  };

}  // namespace ls::event
