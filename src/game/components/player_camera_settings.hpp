#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct PlayerCameraSettings {
    PROPERTY()
    float baseZoom{ 1.f };

    PROPERTY()
    float aimZoom{ 2.f };

    PROPERTY()
    float sprintZoom{ 0.9f };

    PROPERTY()
    float zoomSpeed{ 1.f };

    PROPERTY()
    float baseFollowSpeed{ 3.f };

    PROPERTY()
    float aimFollowSpeed{ 1.f };

    PROPERTY()
    float aimOffsetWeight{ 0.3f };

    PROPERTY()
    float maxAimOffset{ 1.f };
  };

}  // namespace ls::component
