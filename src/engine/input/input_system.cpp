#include "engine/input/input_system.hpp"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

namespace ls::input_system {

  namespace {
    SDL_Scancode toScancode(input::Key key) {
      switch (key) {
        case input::Key::W:
          return SDL_SCANCODE_W;
        case input::Key::A:
          return SDL_SCANCODE_A;
        case input::Key::S:
          return SDL_SCANCODE_S;
        case input::Key::D:
          return SDL_SCANCODE_D;
        case input::Key::Escape:
          return SDL_SCANCODE_ESCAPE;
        case input::Key::LShift:
          return SDL_SCANCODE_LSHIFT;
        case input::Key::Grave:
          return SDL_SCANCODE_GRAVE;
      }

      return SDL_SCANCODE_UNKNOWN;
    }

    SDL_MouseButtonFlags toMouseButtonFlags(input::Button button) {
      switch (button) {
        case input::Button::Left:
          return SDL_BUTTON_LMASK;
        case input::Button::Right:
          return SDL_BUTTON_RMASK;
      }

      return 0;
    }
  }  // namespace

  bool isKeyPressed(input::Key key) {
    const bool* keys{ SDL_GetKeyboardState(nullptr) };
    return keys[toScancode(key)];
  }

  glm::vec2 getMousePosition() {
    glm::vec2 pos{ 0.f };
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
  }

  bool isButtonPressed(input::Button button) {
    const auto buttons{ SDL_GetMouseState(nullptr, nullptr) };
    return (buttons & toMouseButtonFlags(button));
  }

}  // namespace ls::input_system
