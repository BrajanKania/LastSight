#pragma once

namespace ls::component {

  struct PlayerState {
    bool isSprinting{ false };
    bool isAiming{ false };
    bool isMoving{ false };
  };

}  // namespace ls::component
