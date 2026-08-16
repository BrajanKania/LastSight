#pragma once

#include <cstdint>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::gfx {

  STRUCT()
  struct TextureHandle {
    uint32_t id{ 0 };

    bool isValid() const { return id != 0; }
    explicit operator bool() const { return isValid(); }
    bool operator==(const TextureHandle&) const = default;
  };

}  // namespace ls::gfx
