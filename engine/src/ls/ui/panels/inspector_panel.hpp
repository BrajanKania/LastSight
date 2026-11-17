#pragma once

#include <entt/core/fwd.hpp>
#include <entt/entt.hpp>
#include <ls/ui/i_ui_panel.hpp>
#include <string>

#include "ls/core/engine_context.hpp"
#include "ls/reflection/reflection_system.hpp"
#include "ls/ui/ui_context.hpp"

namespace ls::ui {

  class InspectorPanel : public IUIPanel {
  public:
    InspectorPanel() = default;
    void render(const UIContext& ctx) override;

  private:
    void renderEntityInspectorMenuBar(const UIContext& ctx);
    void renderAssetInspectorMenuBar(const UIContext& ctx);

    void renderEntityInspector(const UIContext& ctx);
    void renderAssetInspector(const UIContext& ctx);

    void inspectAsset(const UIContext& ctx);

    void inspectEntity(const UIContext& ctx, const ecs::EntityId entity);
    bool inspectComponentProperty(const EngineContext& ctx, entt::meta_any& owner, entt::meta_data data);

    void renderAddComponentModal(const UIContext& ctx);
    void renderSaveAsPrefabModal(const UIContext& ctx);
    void renderLinkToPrefabModal(const UIContext& ctx);
    void renderSyncFromPrefabModal(const UIContext& ctx);

    bool filterComponent(const std::string& componentName) const;
    bool filterProperty(const reflection_system::PropertyInfo* propInfo) const;

    bool hideReadOnly_{ false };
    bool hideTransient_{ false };
    char filterBuffer_[256]{};
    std::string lowerFilterBuffer_;

    bool shouldOpenAddModal_{ false };
    char addComponentFilter_[256]{};
    std::vector<entt::id_type> selectedComponentsToAdd_{};

    bool shouldOpenSaveAsPrefabModal_{ false };
    char saveAsPrefabNameBuffer_[256]{};

    bool shouldOpenSyncFromPrefabModal_{ false };
    char syncComponentFilter_[256]{};
    std::vector<entt::id_type> selectedComponentsToSync_{};

    bool shouldOpenLinkToPrefabModal_{ false };
    char prefabSearchBuffer_[256]{};
    std::string selectedPrefabName_{};
  };

}  // namespace ls::ui
