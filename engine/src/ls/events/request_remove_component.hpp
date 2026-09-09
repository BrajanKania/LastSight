#pragma once

#include <entt/core/fwd.hpp>

#include "ls/ecs/types.hpp"

namespace ls::event {

  struct RequestRemoveComponent {
    ecs::EntityId entity{ ecs::kNullEntity };
    entt::id_type typeId{ 0 };
  };

}  // namespace ls::event
