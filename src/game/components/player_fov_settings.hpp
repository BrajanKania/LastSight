#pragma once

namespace ls::component {

  struct PlayerFovSettings {
    float baseAngle{ 100.f };
    float aimAngle{ 20.f };
    float lowStaminaAngle{ 50.f };

    float baseSmoothnessAngle{ 25.f };
    float aimSmoothnessAngle{ 5.f };
    float lowStaminaSmoothnessAngle{ 30.f };

    float lowStaminaThreshold{ 0.5f };

    float transitionSpeed{ 10.f };
  };

}  // namespace ls::component
