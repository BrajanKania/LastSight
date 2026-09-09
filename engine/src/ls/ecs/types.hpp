#pragma once

#include <cstdint>
#include <limits>

namespace ls::ecs {

  using EntityId = uint32_t;
  constexpr EntityId kNullEntity{ std::numeric_limits<EntityId>::max() };

  using ComponentId = uint16_t;

}  // namespace ls::ecs
