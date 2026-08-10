#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Lamp {
    bool isOn{ false };
  };

}  // namespace ls::component
