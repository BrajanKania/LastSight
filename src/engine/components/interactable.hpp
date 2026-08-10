#pragma once

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct Interactable {
    float radius{ 1.f };
  };

}  // namespace ls::component
