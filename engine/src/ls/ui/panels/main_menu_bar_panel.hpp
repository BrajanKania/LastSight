#pragma once

#include "ls/ui/i_ui_panel.hpp"

namespace ls::ui {

  class MainMenuBarPanel : public IUIPanel {
  public:
    MainMenuBarPanel() = default;
    void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
