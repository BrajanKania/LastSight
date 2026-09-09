#pragma once

#include <cstdint>
#include <limits>

namespace ls::renderer {

  using MaterialId = std::uint32_t;
  constexpr MaterialId kNullMaterial{ std::numeric_limits<MaterialId>::max() };

  struct MaterialHandle {
    MaterialId id{ kNullMaterial };

    bool isValid() const { return id != kNullMaterial; }
    explicit operator bool() const { return isValid(); }
    bool operator==(const MaterialHandle&) const = default;
  };

}  // namespace ls::renderer
