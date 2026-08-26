#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Lamp {
    PROPERTY()
    bool isOn{ false };
  };

}  // namespace ls::component
