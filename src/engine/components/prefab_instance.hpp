#pragma once

#include "engine/prefab/prefab_handle.hpp"
#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT(DisplayName("Prefab Instance"))
  struct PrefabInstance {
    PROPERTY(DisplayName("Handle"))
    prefab::PrefabHandle handle{};
  };

}  // namespace ls::component
