#pragma once

#include <engine/ui/i_ui_panel.hpp>

#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"

namespace ls::ui {

  class EntityExplorerPanel : public IUIPanel {
  public:
    void render(const UIContext& ctx) override;

  private:
    bool filterEntity(const ecs::Registry& registry, const ecs::EntityId entity);

    bool hideNameless_{ true };
    ecs::EntityId selectedEntity_{ ecs::kNullEntity };
  };

}  // namespace ls::ui
