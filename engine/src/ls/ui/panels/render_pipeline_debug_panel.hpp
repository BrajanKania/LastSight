#pragma once

#include <imgui.h>

#include <glm/ext/vector_float2.hpp>
#include <string>

#include "ls/ui/i_ui_panel.hpp"

namespace ls::ui {

  class RenderPipelineDebugPanel : public IUIPanel {
  public:
    RenderPipelineDebugPanel() = default;
    void render(const UIContext& ctx) override;

  private:
    std::string selectedFramebufferName_{};

    ImVec2 canvasPan_{ 10.0f, 10.0f };
    float canvasZoom_{ 0.3f };
  };

}  // namespace ls::ui
