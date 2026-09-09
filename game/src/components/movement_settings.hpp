#pragma once

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct MovementSettings {
    PROPERTY()
    float walkSpeed{ 1.f };

    PROPERTY()
    float sprintSpeed{ 3.f };

    PROPERTY()
    float angularSpeed{ 270.f };
  };

}  // namespace ls::component
