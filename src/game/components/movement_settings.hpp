#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct MovementSettings {
    float walkSpeed{ 1.f };
    float sprintSpeed{ 3.f };
    float angularSpeed{ 270.f };
  };

}  // namespace ls::component
