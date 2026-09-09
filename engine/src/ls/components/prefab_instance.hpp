#pragma once

#include "ls/prefab/prefab_handle.hpp"
#include "ls/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT(DisplayName("Prefab Instance"))
  struct PrefabInstance {
    PROPERTY(DisplayName("Handle"))
    prefab::PrefabHandle handle{};
  };

}  // namespace ls::component
