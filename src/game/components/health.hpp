#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Health {
    PROPERTY()
    float max{ 100.f };

    PROPERTY()
    float current{ 100.f };
  };

}  // namespace ls::component
