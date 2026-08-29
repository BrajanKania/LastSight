#pragma once

#include <entt/core/fwd.hpp>

#include "engine/ecs/types.hpp"

namespace ls::event {

  struct RequestSyncComponentFromPrefab {
    ecs::EntityId entity{ ecs::kNullEntity };
    entt::id_type typeId{ 0 };
  };

}  // namespace ls::event
