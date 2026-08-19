#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class ConsolePanel : public IUIPanel {
  public:
    ConsolePanel() = default;
    void render(const UIContext& ctx) override;

  private:
    char inputBuffer_[256]{};
    bool reclaimFocus_{ false };
  };

}  // namespace ls::ui
