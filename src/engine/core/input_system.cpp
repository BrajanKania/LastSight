#include "engine/core/input_system.hpp"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>

namespace ls::input_system {

  bool isKeyPressed(SDL_Scancode code) {
    const bool* keys{SDL_GetKeyboardState(nullptr)};
    return keys[code];
  }

}  // namespace ls::input_system
