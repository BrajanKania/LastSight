#pragma once

#include "engine/ecs/types.hpp"
#include "engine/prefab/prefab_handle.hpp"

namespace ls::event {

  struct RequestOverwriteEntityPrefab {
    prefab::PrefabHandle handle{};
    ecs::EntityId entity{ ecs::kNullEntity };
  };

}  // namespace ls::event
