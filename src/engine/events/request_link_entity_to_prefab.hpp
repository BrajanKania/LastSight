#pragma once

#include "engine/ecs/types.hpp"
#include "engine/prefab/prefab_handle.hpp"

namespace ls::event {

  struct RequestLinkEntityToPrefab {
    ecs::EntityId entity{ ecs::kNullEntity };
    prefab::PrefabHandle handle{};
  };

}  // namespace ls::event
