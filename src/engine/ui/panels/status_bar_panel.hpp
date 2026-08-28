#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class StatusBarPanel : public IUIPanel {
  public:
    StatusBarPanel() = default;
    void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
