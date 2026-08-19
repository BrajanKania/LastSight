#pragma once

#include <engine/ui/i_ui_panel.hpp>
#include <entt/entt.hpp>
#include <string>

#include "engine/reflection/reflection_system.hpp"
#include "engine/ui/ui_context.hpp"

namespace ls::ui {

  class EntityInspectorPanel : public IUIPanel {
  public:
    EntityInspectorPanel() = default;
    void render(const UIContext& ctx) override;

  private:
    void inspectEntity(const UIContext& ctx, const ecs::EntityId entity, const gfx::TextureManager& textureManager);
    bool inspectComponentProperty(
        entt::meta_any& owner, entt::meta_data data, const gfx::TextureManager& textureManager
    );

    void renderAddComponentModal(const UIContext& ctx);

    bool filterComponent(const std::string& componentName) const;
    bool filterProperty(const reflection_system::PropertyInfo* propInfo) const;

    bool hideReadOnly_{ false };
    bool hideTransient_{ false };
    char filterBuffer_[256]{};
    std::string lowerFilterBuffer_;

    bool shouldOpenAddModal_{ false };
    char addComponentFilter_[256]{};
    std::vector<entt::id_type> selectedComponentsToAdd_;
  };

}  // namespace ls::ui
