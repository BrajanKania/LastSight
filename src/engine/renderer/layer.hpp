#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::renderer {

  ENUM()
  enum Layer : int {
    Background = -100,
    Ground = 0,
    Entities = 10,
    Foreground = 100,
  };

}  // namespace ls::renderer
