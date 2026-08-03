#include "engine/core/input_system.hpp"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

namespace ls::input_system {

  namespace {
    SDL_Scancode toScancode(Key key) {
      switch (key) {
        case Key::W:
          return SDL_SCANCODE_W;
        case Key::A:
          return SDL_SCANCODE_A;
        case Key::S:
          return SDL_SCANCODE_S;
        case Key::D:
          return SDL_SCANCODE_D;
        case Key::Escape:
          return SDL_SCANCODE_ESCAPE;
        case Key::LShift:
          return SDL_SCANCODE_LSHIFT;
      }

      return SDL_SCANCODE_UNKNOWN;
    }

    SDL_MouseButtonFlags toMouseButtonFlags(Button button) {
      switch (button) {
        case Button::Left:
          return SDL_BUTTON_LMASK;
        case Button::Right:
          return SDL_BUTTON_RMASK;
      }

      return 0;
    }
  }  // namespace

  bool isKeyPressed(Key key) {
    const bool* keys{ SDL_GetKeyboardState(nullptr) };
    return keys[toScancode(key)];
  }

  glm::vec2 getMousePosition() {
    glm::vec2 pos{ 0.f };
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
  }

  bool isButtonPressed(Button button) {
    const auto buttons{ SDL_GetMouseState(nullptr, nullptr) };
    return (buttons & toMouseButtonFlags(button));
  }

}  // namespace ls::input_system
