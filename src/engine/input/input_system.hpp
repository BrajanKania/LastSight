#pragma once

#include <SDL3/SDL_events.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

#include "engine/input/types.hpp"

namespace ls::input_system {

  bool isKeyPressed(input::Key key);

  glm::vec2 getMousePosition();

  glm::vec2 getViewportPosition();

  glm::vec2 getViewportSize();

  void setViewportBounds(const glm::vec2& position, const glm::vec2& size);

  glm::vec2 getViewportMousePosition();

  bool isButtonPressed(input::Button button);

  void processEvent(const SDL_Event& event);

  float getScrollDelta();

  void clearScrollDelta();

}  // namespace ls::input_system
