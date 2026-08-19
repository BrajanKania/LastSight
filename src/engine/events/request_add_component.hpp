#pragma once

#include <entt/core/fwd.hpp>

#include "engine/ecs/types.hpp"

namespace ls::event {

  struct RequestAddComponent {
    ecs::EntityId entity{ ecs::kNullEntity };
    entt::id_type typeId{ 0 };
  };

}  // namespace ls::event
