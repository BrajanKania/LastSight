#pragma once

#include "engine/renderer/render_pipeline.hpp"
#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class RenderPipelineDebugPanel : public IUIPanel {
  public:
    explicit RenderPipelineDebugPanel(RenderPipeline& renderPipeline);

    void render(const UIContext& ctx) override;

  private:
    RenderPipeline& renderPipeline_;

    int selectedPassIndex_{ -1 };
    float imageScale_{ 0.2f };
  };

}  // namespace ls::ui
