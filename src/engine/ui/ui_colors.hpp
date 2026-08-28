#pragma once

#include <imgui.h>

#include "engine/debug/log_level.hpp"

namespace ls::ui::color {

  constexpr ImVec4 forLogLevel(debug::LogLevel level) {
    switch (level) {
      case debug::LogLevel::Error:
        return ImVec4(0.8f, 0.3f, 0.3f, 1.0f);
      case debug::LogLevel::Warning:
        return ImVec4(0.8f, 0.7f, 0.2f, 1.0f);
      case debug::LogLevel::Info:
        return ImVec4(0.3f, 0.7f, 0.3f, 1.0f);
      default:
        return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    }
  }

}  // namespace ls::ui::color
