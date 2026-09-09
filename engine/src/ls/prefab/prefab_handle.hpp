#pragma once

#include <cstdint>
#include <limits>

namespace ls::prefab {

  using PrefabId = std::uint32_t;
  constexpr PrefabId kNullPrefab{ std::numeric_limits<PrefabId>::max() };

  struct PrefabHandle {
    PrefabId id{ kNullPrefab };

    bool isValid() const { return id != kNullPrefab; }
    explicit operator bool() const { return isValid(); }
    bool operator==(const PrefabHandle&) const = default;
  };

}  // namespace ls::prefab
