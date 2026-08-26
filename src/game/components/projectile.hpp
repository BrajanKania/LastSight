#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Projectile {
    PROPERTY()
    float lifetime{ 0.f };
  };

}  // namespace ls::component
