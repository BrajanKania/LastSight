#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class MainMenuPanel : public IUIPanel {
  public:
    MainMenuPanel() = default;
    void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
