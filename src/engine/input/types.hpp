#pragma once

#include <cstdint>

namespace ls::input {

  enum class Key { W, A, S, D, LShift, Escape, Grave, E, Num1, Num2, Num3, Num4, Num5, Num6 };

  enum class Button { Left, Right };

  using ActionId = uint32_t;

  enum class ActionState { Up, JustPressed, Held, JustReleased };

  enum class BindingType { None, Key, Button };

  struct ActionBinding {
    BindingType type{ BindingType::None };
    Key key{};
    Button button{};
  };

  struct Axis2DKeys {
    Key up;
    Key down;
    Key left;
    Key right;
  };

}  // namespace ls::input
