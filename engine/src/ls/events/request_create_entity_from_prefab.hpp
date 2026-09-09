#pragma once

#include "ls/prefab/prefab_handle.hpp"

namespace ls::event {

  struct RequestCreateEntityFromPrefab {
    prefab::PrefabHandle handle{};
  };

}  // namespace ls::event
