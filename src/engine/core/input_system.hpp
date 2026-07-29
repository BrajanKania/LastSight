#pragma once

#include <glm/glm.hpp>

namespace ls::input_system {

  enum class Key { W, A, S, D, LShift, Escape };

  bool isKeyPressed(Key key);

  glm::vec2 getMousePosition();

}  // namespace ls::input_system
