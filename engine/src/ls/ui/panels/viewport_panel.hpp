#pragma once

#include <glm/ext/vector_float2.hpp>

#include "ls/ui/i_ui_panel.hpp"

namespace ls::ui {

  class ViewportPanel : public IUIPanel {
  public:
    ViewportPanel() = default;

    virtual void render(const UIContext& ctx) override;

    bool isHovered() const { return isHovered_; }
    bool isFocused() const { return isFocused_; }

  private:
    bool isHovered_{ false };
    bool isFocused_{ false };
    glm::vec2 size_{ 0.f };
  };

}  // namespace ls::ui
