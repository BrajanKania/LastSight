#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct PlayerState {
    PROPERTY()
    bool isSprinting{ false };

    PROPERTY()
    bool isAiming{ false };

    PROPERTY()
    bool isMoving{ false };
  };

}  // namespace ls::component
