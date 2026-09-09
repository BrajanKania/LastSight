#pragma once

#include "ls/ecs/types.hpp"
#include "ls/prefab/prefab_handle.hpp"

namespace ls::event {

  struct RequestOverwriteEntityPrefab {
    prefab::PrefabHandle handle{};
    ecs::EntityId entity{ ecs::kNullEntity };
  };

}  // namespace ls::event
