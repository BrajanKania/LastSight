#pragma once

#include "ls/ecs/types.hpp"

namespace ls::ui {

  struct SelectionContext {
    ecs::EntityId selectedEntity{ ecs::kNullEntity };
  };

}  // namespace ls::ui
