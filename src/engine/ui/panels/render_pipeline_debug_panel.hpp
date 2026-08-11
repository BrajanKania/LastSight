#pragma once

#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class RenderPipelineDebugPanel : public IUIPanel {
  public:
    RenderPipelineDebugPanel() = default;
    void render(const UIContext& ctx) override;

  private:
    int selectedPassIndex_{ -1 };
    float imageScale_{ 0.2f };
  };

}  // namespace ls::ui
