#pragma once

#include "items/item_registry.hpp"
#include "ls/ui/i_ui_panel.hpp"

namespace ls::ui {

  class InventoryPanel : public IUIPanel {
  public:
    explicit InventoryPanel(const item::ItemRegistry& itemRegistry);
    void render(const UIContext& ctx) override;

  private:
    const item::ItemRegistry& itemRegistry_;
  };

}  // namespace ls::ui
