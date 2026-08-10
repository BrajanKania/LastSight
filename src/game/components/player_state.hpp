#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct PlayerState {
    bool isSprinting{ false };
    bool isAiming{ false };
    bool isMoving{ false };
  };

}  // namespace ls::component
