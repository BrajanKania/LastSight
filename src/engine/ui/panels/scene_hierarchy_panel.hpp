#pragma once

#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/ui/i_ui_panel.hpp"

namespace ls::ui {

  class SceneHierarchyPanel : public IUIPanel {
  public:
    SceneHierarchyPanel() = default;
    void render(const UIContext& ctx) override;

  private:
    bool filterEntity(const ecs::Registry& registry, const ecs::EntityId entity) const;

    bool hideNameless_{ true };
  };

}  // namespace ls::ui
