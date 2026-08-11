#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class ToolboxPanel : public IUIPanel {
  public:
    ToolboxPanel() = default;
    void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
