#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class DebugToolboxPanel : public IUIPanel {
  public:
    void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
