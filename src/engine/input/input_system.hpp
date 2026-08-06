#pragma once

#include <SDL3/SDL_events.h>

#include <glm/glm.hpp>

#include "engine/input/types.hpp"

namespace ls::input_system {

  bool isKeyPressed(input::Key key);

  glm::vec2 getMousePosition();

  bool isButtonPressed(input::Button button);

  void processEvent(const SDL_Event& event);

  float getScrollDelta();

  void clearScrollDelta();

}  // namespace ls::input_system
