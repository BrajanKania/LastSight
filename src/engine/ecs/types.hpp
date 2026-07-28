#pragma once

#include <cstdint>
#include <limits>

namespace ls::ecs {

  using Entity = uint32_t;
  constexpr Entity kNullEntity{std::numeric_limits<Entity>::max()};

  using ComponentType = uint16_t;

}  // namespace ls::ecs
