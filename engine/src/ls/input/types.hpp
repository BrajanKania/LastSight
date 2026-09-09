#pragma once

#include <cstdint>
#include <unordered_set>

namespace ls::input {

  enum class Key {
    W,
    A,
    S,
    D,
    E,

    LShift,
    RShift,

    Escape,
    Grave,

    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,

    LCtrl,
    RCtrl,

    LAlt,
    RAlt,

    Delete,
  };

  enum class Button {
    Left,
    Right,
  };

  using Modifier = uint8_t;

  enum class KeyModifier : Modifier {
    None = 0,
    Ctrl = 1 << 0,
    Shift = 1 << 1,
    Alt = 1 << 2,
  };

  constexpr Modifier operator|(KeyModifier a, KeyModifier b) {
    return static_cast<Modifier>(a) | static_cast<Modifier>(b);
  }

  constexpr Modifier operator|(Modifier a, KeyModifier b) { return a | static_cast<Modifier>(b); }

  constexpr Modifier operator|=(Modifier& a, KeyModifier b) {
    a |= static_cast<Modifier>(b);
    return a;
  }

  using ActionId = uint32_t;

  enum class ActionState {
    Up,
    JustPressed,
    Held,
    JustReleased,
  };

  enum class BindingType {
    None,
    Key,
    Button,
  };

  struct ActionBinding {
    BindingType type{ BindingType::None };
    Key key{};
    Button button{};
    Modifier modifiers{ 0 };
  };

  struct Axis2DKeys {
    Key up;
    Key down;
    Key left;
    Key right;
  };

  struct ConsumedInputState {
    std::unordered_set<Key> keys{};
    std::unordered_set<Button> buttons{};
  };

}  // namespace ls::input
