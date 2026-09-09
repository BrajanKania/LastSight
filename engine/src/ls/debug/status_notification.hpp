#pragma once

#include <string>

#include "ls/debug/log_level.hpp"

namespace ls::debug {

  namespace status_duration {
    inline constexpr float kShort{ 1.5f };
    inline constexpr float kMedium{ 3.5f };
    inline constexpr float kLong{ 6.f };
  }  // namespace status_duration

  struct StatusNotification {
    std::string message{ "Ready" };
    float timeRemaining{ status_duration::kMedium };
    LogLevel level{ LogLevel::Info };
  };

}  // namespace ls::debug
