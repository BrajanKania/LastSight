#pragma once

#include "engine/ui/ui_context.hpp"

namespace ls::ui {

  class IUIPanel {
  public:
    virtual ~IUIPanel() = default;

    virtual void render(const UIContext& ctx) = 0;

    bool isVisible() const { return visible_; }
    void setVisible(bool visible) { visible_ = visible; }
    void toggleVisible() { visible_ = !visible_; }

  protected:
    bool visible_{ false };
  };

}  // namespace ls::ui
