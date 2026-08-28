#pragma once

#include "engine/input/types.hpp"

namespace ls::input {

  struct InputContext {
    bool captureKeyboard{ false };
    bool captureMouse{ false };
    ConsumedInputState& consumedInputState;
  };

}  // namespace ls::input
