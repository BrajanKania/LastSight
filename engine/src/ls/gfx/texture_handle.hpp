#pragma once

#include <cstdint>

namespace ls::gfx {

  struct TextureHandle {
    uint32_t id{ 0 };

    bool isValid() const { return id != 0; }
    explicit operator bool() const { return isValid(); }
    bool operator==(const TextureHandle&) const = default;
  };

}  // namespace ls::gfx
