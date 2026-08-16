#pragma once

#include <engine/ui/i_ui_panel.hpp>

namespace ls::ui {

  class EntityInspectorPanel : public IUIPanel {
  public:
    EntityInspectorPanel() = default;
    void render(const UIContext& ctx) override;
  };

}  // namespace ls::ui
