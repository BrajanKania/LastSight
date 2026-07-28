#pragma once

namespace ls::input_system {

  enum class Key { W, A, S, D, LShift, Escape };

  bool isKeyPressed(Key key);

}  // namespace ls::input_system
