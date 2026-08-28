#include "engine/debug/status_bar.hpp"

#include "engine/debug/log_level.hpp"
#include "engine/debug/status_notification.hpp"

namespace ls::debug {

  void StatusBar::showMessage(const std::string& message, float duration, LogLevel level) {
    notification_ = StatusNotification{
      .message = message,
      .timeRemaining = duration,
      .level = level,
    };
  }

  void StatusBar::update(float dt) {
    if (notification_.timeRemaining > 0.f) {
      notification_.timeRemaining -= dt;
      if (notification_.timeRemaining <= 0.f) {
        notification_ = StatusNotification{
          .message = "Ready",
          .timeRemaining = 0.f,
          .level = LogLevel::Info,
        };
      }
    }
  }

}  // namespace ls::debug
