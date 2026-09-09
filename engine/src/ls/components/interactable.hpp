#pragma once

#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Interactable {
    PROPERTY()
    float radius{ 1.f };
  };

}  // namespace ls::component
