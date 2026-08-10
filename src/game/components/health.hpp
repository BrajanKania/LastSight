#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Health {
    float max{ 100.f };
    float current{ 100.f };
  };

}  // namespace ls::component
