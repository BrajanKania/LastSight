#pragma once

#include "ls/ui/ui_style.hpp"

namespace ls::ui {

  struct EditorPreferences {
    UIStyle uiStyle{ ui::UIStyle::Dark };
    bool consoleAutoScroll{ true };
  };

}  // namespace ls::ui
