#pragma once

#include "engine/debug/log_level.hpp"
#include "engine/debug/status_notification.hpp"

namespace ls::debug {

  class StatusBar {
  public:
    void showMessage(const std::string& message, float duration, LogLevel level = LogLevel::Info);
    void update(float dt);
    const StatusNotification& getNotification() const { return notification_; }

  private:
    StatusNotification notification_{ .timeRemaining = 0.f };
  };

}  // namespace ls::debug
