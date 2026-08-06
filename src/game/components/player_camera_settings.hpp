#pragma once

namespace ls::component {

  struct PlayerCameraSettings {
    float baseZoom{ 1.f };
    float aimZoom{ 2.f };
    float sprintZoom{ 0.9f };

    float zoomSpeed{ 1.f };

    float baseFollowSpeed{ 3.f };
    float aimFollowSpeed{ 1.f };

    float aimOffsetWeight{ 0.3f };
    float maxAimOffset{ 1.f };
  };

}  // namespace ls::component
