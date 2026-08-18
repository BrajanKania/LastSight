#pragma once

#include "engine/ui/ui_style.hpp"

namespace ls::event {

  struct RequestChangeUIStyle {
    ui::UIStyle newStyle{ ui::UIStyle::Dark };
  };

}  // namespace ls::event
