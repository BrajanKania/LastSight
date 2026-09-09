#pragma once

#include "ls/ecs/types.hpp"
#include "ls/prefab/prefab_handle.hpp"

namespace ls::event {

  struct RequestLinkEntityToPrefab {
    ecs::EntityId entity{ ecs::kNullEntity };
    prefab::PrefabHandle handle{};
  };

}  // namespace ls::event
