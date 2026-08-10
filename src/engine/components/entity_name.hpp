#pragma once

#include <string>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  STRUCT()
  struct EntityName {
    std::string name{};
  };

}  // namespace ls::component
