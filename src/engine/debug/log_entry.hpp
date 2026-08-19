#pragma once

#include <string>

#include "engine/debug/log_level.hpp"

namespace ls::debug {

  struct LogEntry {
    std::string message;
    LogLevel level{ LogLevel::Info };
  };

}  // namespace ls::debug
