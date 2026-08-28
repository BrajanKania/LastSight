#include "engine/input/input_system.hpp"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

#include <algorithm>
#include <glm/ext/vector_float2.hpp>

#include "engine/input/types.hpp"

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
        case input::Key::E:
          return SDL_SCANCODE_E;
        case input::Key::Escape:
          return SDL_SCANCODE_ESCAPE;
        case input::Key::LShift:
          return SDL_SCANCODE_LSHIFT;
        case input::Key::RShift:
          return SDL_SCANCODE_RSHIFT;
        case input::Key::Grave:
          return SDL_SCANCODE_GRAVE;
        case input::Key::Num1:
          return SDL_SCANCODE_1;
        case input::Key::Num2:
          return SDL_SCANCODE_2;
        case input::Key::Num3:
          return SDL_SCANCODE_3;
        case input::Key::Num4:
          return SDL_SCANCODE_4;
        case input::Key::Num5:
          return SDL_SCANCODE_5;
        case input::Key::Num6:
          return SDL_SCANCODE_6;
        case input::Key::LCtrl:
          return SDL_SCANCODE_LCTRL;
        case input::Key::RCtrl:
          return SDL_SCANCODE_RCTRL;
        case input::Key::LAlt:
          return SDL_SCANCODE_LALT;
        case input::Key::RAlt:
          return SDL_SCANCODE_RALT;
        case input::Key::Delete:
          return SDL_SCANCODE_DELETE;
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

    float scrollDeltaY{ 0.f };
    glm::vec2 viewportPosition{ 0.f, 0.f };
    glm::vec2 viewportSize{ 0.f, 0.f };
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

  glm::vec2 getViewportPosition() { return viewportPosition; }

  glm::vec2 getViewportSize() { return viewportSize; }

  void setViewportBounds(const glm::vec2& position, const glm::vec2& size) {
    viewportPosition = position;
    viewportSize = size;
  }

  glm::vec2 getViewportMousePosition() {
    glm::vec2 globalMouse{ getMousePosition() };
    glm::vec2 localMouse{ globalMouse - viewportPosition };

    if (viewportSize.x > 0.f && viewportSize.y > 0.f) {
      localMouse.x = std::clamp(localMouse.x, 0.f, viewportSize.x);
      localMouse.y = std::clamp(localMouse.y, 0.f, viewportSize.y);
    }

    return localMouse;
  }

  bool isButtonPressed(input::Button button) {
    const auto buttons{ SDL_GetMouseState(nullptr, nullptr) };
    return (buttons & toMouseButtonFlags(button));
  }

  void processEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
      scrollDeltaY += event.wheel.y;
    }
  }

  float getScrollDelta() { return scrollDeltaY; }

  void clearScrollDelta() { scrollDeltaY = 0.f; }

}  // namespace ls::input_system
