#pragma once

#include <string>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT(DisplayName("Entity Name"))
  struct EntityName {
    PROPERTY(DisplayName("Name"))
    std::string name{};
  };

}  // namespace ls::component
