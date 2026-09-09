#pragma once

#include "ls/input/types.hpp"

namespace ls::input {

  struct InputContext {
    bool captureKeyboard{ false };
    bool captureMouse{ false };
    ConsumedInputState& consumedInputState;
  };

}  // namespace ls::input
