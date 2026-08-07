#pragma once

namespace ls::component {

  struct FieldOfView {
    float innerRadius{ 1.f };
    float outerRadius{ 10.f };
    float fovAngle{ 60.f };
    float smoothnessAngle{ 4.f };
    float smoothnessDistance{ 1.f };
    float darkness{ 1.f };
  };

}  // namespace ls::component
