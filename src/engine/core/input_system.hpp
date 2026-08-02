#pragma once

#include <glm/glm.hpp>

namespace ls::input_system {

  enum class Key { W, A, S, D, LShift, Escape };
  enum class Button { Left, Right };

  bool isKeyPressed(Key key);

  glm::vec2 getMousePosition();

  bool isButtonPressed(Button button);

}  // namespace ls::input_system
