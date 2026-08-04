#pragma once

#include <glm/glm.hpp>

#include "engine/input/types.hpp"

namespace ls::input_system {

  bool isKeyPressed(input::Key key);

  glm::vec2 getMousePosition();

  bool isButtonPressed(input::Button button);

}  // namespace ls::input_system
