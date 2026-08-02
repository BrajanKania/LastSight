#pragma once

#include <SDL3/SDL_timer.h>

namespace ls::time_system {

  inline uint64_t ms() { return SDL_GetTicks(); }

  inline float sec() { return static_cast<float>(ms()) / 1000.0; }

}  // namespace ls::time_system
