#pragma once

#include <string>

#include "engine/ecs/types.hpp"

namespace ls::event {

  struct RequestSaveEntityAsPrefab {
    std::string name{};
    ecs::EntityId entity{ ecs::kNullEntity };
  };

}  // namespace ls::event
