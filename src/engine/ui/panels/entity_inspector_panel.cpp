#include "engine/ui/panels/entity_inspector_panel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <entt/core/fwd.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <format>
#include <glm/glm.hpp>
#include <string>

#include "engine/components/prefab_instance.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/request_add_component.hpp"
#include "engine/events/request_decouple_entity_from_prefab.hpp"
#include "engine/events/request_link_entity_to_prefab.hpp"
#include "engine/events/request_overwrite_entity_prefab.hpp"
#include "engine/events/request_remove_component.hpp"
#include "engine/events/request_save_entity_as_prefab.hpp"
#include "engine/events/request_sync_component_from_prefab.hpp"
#include "engine/gfx/texture_handle.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/prefab/prefab_handle.hpp"
#include "engine/prefab/prefab_manager.hpp"
#include "engine/reflection/reflection_system.hpp"
#include "engine/renderer/material/material_handle.hpp"
#include "engine/renderer/material/material_manager.hpp"
#include "engine/ui/panels/panel_names.hpp"
#include "engine/ui/ui_context.hpp"

namespace ls::ui {

  void EntityInspectorPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.eventQueue != nullptr && "[EntityInspectorPanel] Requires a valid EventQueue!");
    assert(ctx.selectionCtx != nullptr && "[EntityInspectorPanel] Requires a valid SelectionContext!");
    assert(ctx.sceneCtx.registry != nullptr && "[EntityInspectorPanel] Requires a valid Registry!");
    assert(ctx.engineCtx.textureManager != nullptr && "[EntityInspectorPanel] Requires a valid TextureManager!");
    assert(ctx.engineCtx.prefabManager != nullptr && "[EntityInspectorPanel] Requires a valid PrefabManager!");
    assert(ctx.engineCtx.materialManager != nullptr && "[EntityInspectorPanel] Requires a valid MaterialManager!");

    if (!ctx.sceneCtx.registry->isValidEntity(ctx.selectionCtx->selectedEntity)) {
      ctx.selectionCtx->selectedEntity = ecs::kNullEntity;
    }

    renderAddComponentModal(ctx);
    renderSaveAsPrefabModal(ctx);
    renderLinkToPrefabModal(ctx);
    renderSyncFromPrefabModal(ctx);

    const bool hasSelectedEntity{ ctx.selectionCtx->selectedEntity != ecs::kNullEntity };

    ImGuiWindowFlags flags{ ImGuiWindowFlags_MenuBar };

    if (ImGui::Begin(ui::panel::kEntityInspector, &visible_, flags)) {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::MenuItem("Add Component", nullptr, false, hasSelectedEntity)) {
          shouldOpenAddModal_ = true;
        }

        bool isSelectedEntityPrefabInstance{
          hasSelectedEntity &&
          ctx.sceneCtx.registry->hasComponent<component::PrefabInstance>(ctx.selectionCtx->selectedEntity) &&
          ctx.sceneCtx.registry->getComponent<component::PrefabInstance>(ctx.selectionCtx->selectedEntity)
              .handle.isValid()
        };

        if (ImGui::BeginMenu("Prefab")) {
          if (ImGui::MenuItem("Sync", nullptr, false, isSelectedEntityPrefabInstance)) {
            shouldOpenSyncFromPrefabModal_ = true;
          }
          if (ImGui::MenuItem("Overwrite", nullptr, false, isSelectedEntityPrefabInstance)) {
            auto prefabInstance{
              ctx.sceneCtx.registry->getComponent<component::PrefabInstance>(ctx.selectionCtx->selectedEntity)
            };

            ctx.engineCtx.eventQueue->publish(
                event::RequestOverwriteEntityPrefab{
                    .handle = prefabInstance.handle,
                    .entity = ctx.selectionCtx->selectedEntity,
                }
            );
          }
          if (ImGui::MenuItem("Decouple", nullptr, false, isSelectedEntityPrefabInstance)) {
            ctx.engineCtx.eventQueue->publish(
                event::RequestDecoupleEntityFromPrefab{ .entity = ctx.selectionCtx->selectedEntity }
            );
          }
          ImGui::Separator();
          if (ImGui::MenuItem(
                  "Link to Prefab",
                  nullptr,
                  isSelectedEntityPrefabInstance,
                  hasSelectedEntity && !isSelectedEntityPrefabInstance
              )) {
            shouldOpenLinkToPrefabModal_ = true;
          }
          if (ImGui::MenuItem("Save as Prefab", nullptr, false, hasSelectedEntity)) {
            shouldOpenSaveAsPrefabModal_ = true;
          }
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
          ImGui::MenuItem("Hide Read Only", nullptr, &hideReadOnly_);
          ImGui::MenuItem("Hide Transient", nullptr, &hideTransient_);

          ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
      }

      {  // Filter
        ImGui::SetNextItemWidth(-1.f);
        ImGui::InputTextWithHint("##InputFilterBuffer", "Search components...", filterBuffer_, sizeof(filterBuffer_));
        ImGui::Separator();
      }

      {  // Inspector
        lowerFilterBuffer_ = filterBuffer_;
        std::transform(
            lowerFilterBuffer_.begin(), lowerFilterBuffer_.end(), lowerFilterBuffer_.begin(), [](unsigned char ch) {
              return std::tolower(ch);
            }
        );

        ImGui::BeginChild("InspectorRegion");

        if (ctx.sceneCtx.registry->isValidEntity(ctx.selectionCtx->selectedEntity)) {
          inspectEntity(
              ctx,
              ctx.selectionCtx->selectedEntity,
              *ctx.engineCtx.materialManager,
              *ctx.engineCtx.textureManager,
              *ctx.engineCtx.prefabManager
          );

          if (ImGui::BeginPopupContextWindow(
                  "InspectorBlankContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems
              )) {
            if (ImGui::MenuItem("Add Component")) {
              shouldOpenAddModal_ = true;
            }
            ImGui::EndPopup();
          }
        } else {
          ImGui::Text("Select an entity from the list");
        }

        ImGui::EndChild();
      }
    }
    ImGui::End();
  }

  void EntityInspectorPanel::inspectEntity(
      const UIContext& ctx,
      const ecs::EntityId entity,
      const renderer::MaterialManager& materialManager,
      const gfx::TextureManager& textureManager,
      const prefab::PrefabManager& prefabManager
  ) {
    for (auto [typeId, type] : entt::resolve()) {
      const reflection_system::ComponentInfo* componentInfo{ type.custom() };

      if (componentInfo && !componentInfo->isComponent)
        continue;

      auto* set{ ctx.sceneCtx.registry->getISparseSetByTypeId(typeId) };
      if (!set)
        continue;

      void* rawComponent{ set->getRawComponent(entity) };
      if (!rawComponent)
        continue;

      entt::meta_any anyComponent{ type.from_void(rawComponent) };

      const char* headerName{ componentInfo && componentInfo->displayName ? componentInfo->displayName
                              : type.name()                               ? type.name()
                                                                          : "Unknown" };

      if (!filterComponent(headerName))
        continue;

      bool isHeaderOpen{ ImGui::CollapsingHeader(headerName) };

      if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Remove Component")) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestRemoveComponent{
                  .entity = ctx.selectionCtx->selectedEntity,
                  .typeId = typeId,
              }
          );
        }
        ImGui::EndPopup();
      }

      if (isHeaderOpen) {
        ImGui::PushID(static_cast<int>(typeId));

        ImGuiTableFlags tableFlags{ ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV |
                                    ImGuiTableFlags_SizingStretchProp };

        if (ImGui::BeginTable("ComponentTable", 2, tableFlags)) {
          ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.35f);
          ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.65f);

          for (auto [dataId, data] : type.data()) {
            const ls::reflection_system::PropertyInfo* propInfo{ data.custom() };
            const reflection_system::EnumInfo* enumInfo{ data.custom() };

            if (!filterProperty(propInfo))
              continue;

            ImGui::PushID(static_cast<int>(dataId));

            ImGui::TableNextRow();

            ImGui::TableNextColumn();

            const char* label{ nullptr };
            if (propInfo && propInfo->displayName) {
              label = propInfo->displayName;
            } else if (enumInfo && enumInfo->displayName) {
              label = enumInfo->displayName;
            } else {
              label = data.name() ? data.name() : "Unnamed";
            }

            ImGui::TextUnformatted(label);

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1.0f);

            inspectComponentProperty(anyComponent, data, materialManager, textureManager, prefabManager);

            ImGui::PopID();
          }

          ImGui::EndTable();
        }

        ImGui::PopID();
      }
    }
  }

  bool EntityInspectorPanel::inspectComponentProperty(
      entt::meta_any& owner,
      entt::meta_data data,
      const renderer::MaterialManager& materialManager,
      const gfx::TextureManager& textureManager,
      const prefab::PrefabManager& prefabManager
  ) {
    const ls::reflection_system::PropertyInfo* propertyInfo{ data.custom() };
    const bool isReadOnly{ propertyInfo ? propertyInfo->readOnly : false };
    const bool isColor{ propertyInfo ? propertyInfo->color : false };
    const float step{ propertyInfo ? propertyInfo->step : 0.05f };
    const bool hasRange{ propertyInfo ? propertyInfo->range : false };
    const float rangeMin{ propertyInfo ? propertyInfo->rangeMin : 0.f };
    const float rangeMax{ propertyInfo ? propertyInfo->rangeMax : 1.f };

    const char* label{ (propertyInfo && propertyInfo->displayName) ? propertyInfo->displayName
                                                                   : (data.name() ? data.name() : "Unnamed") };

    entt::meta_any value{ data.get(owner) };

    if (!value)
      return false;

    bool valueChanged{ false };

    if (isReadOnly) {
      ImGui::BeginDisabled(true);
    }

    std::string hiddenLabel{ std::format("##{}", label) };

    auto valueType{ value.type() };

    if (valueType == entt::resolve<gfx::TextureHandle>()) {
      gfx::TextureHandle handle{ value.cast<gfx::TextureHandle>() };
      std::string textureName{ textureManager.getName(handle) };

      if (ImGui::BeginCombo(hiddenLabel.c_str(), textureName.empty() ? "None" : textureName.c_str())) {
        int textureIdx{ 0 };
        for (const auto& name : textureManager.getNames()) {
          ImGui::PushID(textureIdx++);
          const char* displayName{ name.empty() ? "None" : name.c_str() };

          bool isSelected{ textureName == name };
          if (ImGui::Selectable(displayName, isSelected)) {
            value = textureManager.getHandle(name);
            valueChanged = true;
          }

          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
          ImGui::PopID();
        }
        ImGui::EndCombo();
      }
    } else if (valueType == entt::resolve<renderer::MaterialHandle>()) {
      renderer::MaterialHandle handle{ value.cast<renderer::MaterialHandle>() };
      std::string materialName{ materialManager.getName(handle) };

      if (ImGui::BeginCombo(hiddenLabel.c_str(), materialName.empty() ? "None" : materialName.c_str())) {
        int textureIdx{ 0 };
        for (const auto& name : materialManager.getNames()) {
          ImGui::PushID(textureIdx++);
          const char* displayName{ name.empty() ? "None" : name.c_str() };

          bool isSelected{ materialName == name };
          if (ImGui::Selectable(displayName, isSelected)) {
            value = materialManager.getHandle(name);
            valueChanged = true;
          }

          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
          ImGui::PopID();
        }
        ImGui::EndCombo();
      }
    } else if (valueType == entt::resolve<prefab::PrefabHandle>()) {
      prefab::PrefabHandle handle{ value.cast<prefab::PrefabHandle>() };
      auto prefabInfo{ prefabManager.getPrefabInfo(handle) };
      std::string prefabName{ prefabInfo ? prefabInfo->name : "Unnamed" };

      if (ImGui::BeginCombo(hiddenLabel.c_str(), prefabName.c_str())) {
        int prefabIdx{ 0 };
        for (const auto& name : prefabManager.getNames()) {
          ImGui::PushID(prefabIdx++);
          const char* displayName{ name.empty() ? "None" : name.c_str() };

          bool isSelected{ prefabName == name };
          if (ImGui::Selectable(displayName, isSelected)) {
            value = prefabManager.getHandle(name);
            valueChanged = true;
          }

          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
          ImGui::PopID();
        }
        ImGui::EndCombo();
      }
    } else if (valueType == entt::resolve<bool>()) {
      bool val{ value.cast<bool>() };
      if (ImGui::Checkbox(hiddenLabel.c_str(), &val)) {
        value = val;
        valueChanged = true;
      }
    } else if (valueType == entt::resolve<float>()) {
      float val{ value.cast<float>() };
      if (hasRange && ImGui::DragFloat(hiddenLabel.c_str(), &val, step, rangeMin, rangeMax)) {
        value = val;
        valueChanged = true;
      } else if (!hasRange && ImGui::DragFloat(hiddenLabel.c_str(), &val, step)) {
        value = val;
        valueChanged = true;
      }
    } else if (valueType == entt::resolve<int>()) {
      int val{ value.cast<int>() };
      if (ImGui::DragInt(hiddenLabel.c_str(), &val, (int)step)) {
        value = val;
        valueChanged = true;
      }
    } else if (valueType == entt::resolve<uint32_t>()) {
      uint32_t val{ value.cast<uint32_t>() };
      if (ImGui::DragScalar(hiddenLabel.c_str(), ImGuiDataType_U32, &val, (int)step)) {
        value = val;
        valueChanged = true;
      }
    } else if (valueType == entt::resolve<std::size_t>()) {
      std::size_t val{ value.cast<std::size_t>() };
      if (ImGui::DragScalar(hiddenLabel.c_str(), ImGuiDataType_U64, &val, (int)step)) {
        value = val;
        valueChanged = true;
      }
    } else if (auto* str{ value.try_cast<std::string>() }) {
      char buffer[256];
      std::strncpy(buffer, str->c_str(), sizeof(buffer));
      buffer[sizeof(buffer) - 1] = '\0';
      if (ImGui::InputText(hiddenLabel.c_str(), buffer, sizeof(buffer))) {
        value = std::string(buffer);
        valueChanged = true;
      }
    } else if (auto* vec{ value.try_cast<glm::vec2>() }) {
      if (ImGui::DragFloat2(hiddenLabel.c_str(), &vec->x, step)) {
        valueChanged = true;
      }
    } else if (auto* vec{ value.try_cast<glm::vec3>() }) {
      if (isColor) {
        if (ImGui::ColorEdit3(hiddenLabel.c_str(), &vec->x)) {
          valueChanged = true;
        }
      } else if (ImGui::DragFloat3(hiddenLabel.c_str(), &vec->x, step)) {
        valueChanged = true;
      }
    } else if (auto* vec{ value.try_cast<glm::vec4>() }) {
      if (isColor) {
        if (ImGui::ColorEdit4(hiddenLabel.c_str(), &vec->x)) {
          valueChanged = true;
        }
      } else if (ImGui::DragFloat4(hiddenLabel.c_str(), &vec->x, step)) {
        valueChanged = true;
      }
    } else if (valueType.is_enum()) {
      std::string currentEnumName{ "Unknown" };

      for (auto [subDataId, enumData] : valueType.data()) {
        entt::meta_any enumVal{ enumData.get({}) };
        if (enumVal == value) {
          currentEnumName = enumData.name() ? enumData.name() : "Unknown";
          break;
        }
      }

      if (ImGui::BeginCombo(hiddenLabel.c_str(), currentEnumName.c_str())) {
        for (auto [subDataId, enumData] : valueType.data()) {
          ImGui::PushID(static_cast<int>(subDataId));
          entt::meta_any enumVal{ enumData.get({}) };
          const char* itemName{ enumData.name() ? enumData.name() : "Unnamed" };
          const bool isSelected{ enumVal == value };

          if (ImGui::Selectable(itemName, isSelected)) {
            value = enumVal;
            valueChanged = true;
          }

          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
          ImGui::PopID();
        }
        ImGui::EndCombo();
      }
    } else if (valueType.data().begin() != valueType.data().end()) {
      if (ImGui::TreeNode(hiddenLabel.c_str())) {
        for (auto [subDataId, subData] : valueType.data()) {
          ImGui::PushID(static_cast<int>(subDataId));
          if (inspectComponentProperty(value, subData, materialManager, textureManager, prefabManager)) {
            valueChanged = true;
          }
          ImGui::PopID();
        }
        ImGui::TreePop();
      }
    }

    if (isReadOnly) {
      ImGui::EndDisabled();
    }

    if (valueChanged && !isReadOnly) {
      data.set(owner, value);
    }

    return valueChanged && !isReadOnly;
  }

  void EntityInspectorPanel::renderAddComponentModal(const UIContext& ctx) {
    if (shouldOpenAddModal_) {
      ImGui::OpenPopup("Add Component");
      shouldOpenAddModal_ = false;
      selectedComponentsToAdd_.clear();
      addComponentFilter_[0] = '\0';
    }

    if (ImGui::BeginPopupModal("Add Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputTextWithHint(
          "##SearchComponent", "Search component...", addComponentFilter_, sizeof(addComponentFilter_)
      );
      ImGui::Separator();

      ImGui::BeginChild("ComponentListRegion", ImVec2(350.0f, 300.0f), ImGuiChildFlags_Borders);

      std::string filterLower{ addComponentFilter_ };
      std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), [](unsigned char ch) {
        return std::tolower(ch);
      });

      for (auto [typeId, type] : entt::resolve()) {
        const reflection_system::ComponentInfo* componentInfo{ type.custom() };

        if (componentInfo && !componentInfo->isComponent)
          continue;

        auto* set{ ctx.sceneCtx.registry->getISparseSetByTypeId(typeId) };

        if (!set)
          continue;

        if (set->hasComponent(ctx.selectionCtx->selectedEntity))
          continue;

        const char* typeName{ componentInfo->displayName ? componentInfo->displayName
                              : type.name()              ? type.name()
                                                         : "Unknown" };
        std::string nameLower{ typeName };
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), [](unsigned char ch) {
          return std::tolower(ch);
        });

        if (!filterLower.empty() && nameLower.find(filterLower) == std::string::npos) {
          continue;
        }

        auto it{ std::find(selectedComponentsToAdd_.begin(), selectedComponentsToAdd_.end(), typeId) };
        bool isSelected{ it != selectedComponentsToAdd_.end() };

        if (ImGui::Checkbox(typeName, &isSelected)) {
          if (isSelected) {
            selectedComponentsToAdd_.push_back(typeId);
          } else {
            selectedComponentsToAdd_.erase(it);
          }
        }
      }
      ImGui::EndChild();

      ImGui::Separator();

      const bool hasComponentsSelected{ !selectedComponentsToAdd_.empty() };
      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      {  // Add button
        if (!hasComponentsSelected)
          ImGui::BeginDisabled();

        if (ImGui::Button("Add", ImVec2(buttonWidth, 0.0f))) {
          for (auto typeId : selectedComponentsToAdd_) {
            ctx.engineCtx.eventQueue->publish(
                event::RequestAddComponent{
                    .entity = ctx.selectionCtx->selectedEntity,
                    .typeId = typeId,
                }
            );
          }
          selectedComponentsToAdd_.clear();
          addComponentFilter_[0] = '\0';
          ImGui::CloseCurrentPopup();
        }

        if (!hasComponentsSelected)
          ImGui::EndDisabled();
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        selectedComponentsToAdd_.clear();
        addComponentFilter_[0] = '\0';
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void EntityInspectorPanel::renderSaveAsPrefabModal(const UIContext& ctx) {
    if (shouldOpenSaveAsPrefabModal_) {
      ImGui::OpenPopup("Save as Prefab");
      saveAsPrefabNameBuffer_[0] = '\0';
      shouldOpenSaveAsPrefabModal_ = false;
    }

    if (ImGui::BeginPopupModal("Save as Prefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::SetNextItemWidth(200.f);
      ImGui::InputTextWithHint(
          "##NameInput", "Enter name...", saveAsPrefabNameBuffer_, sizeof(saveAsPrefabNameBuffer_)
      );

      ImGui::Separator();

      std::string name{ saveAsPrefabNameBuffer_ };
      const bool hasWhitespace{ std::ranges::any_of(name, [](unsigned char ch) { return std::isspace(ch); }) };

      const bool isValidName{ !name.empty() && !hasWhitespace };
      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      {  // Save Button
        if (!isValidName)
          ImGui::BeginDisabled();

        if (ImGui::Button("Save", ImVec2(buttonWidth, 0.0f))) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestSaveEntityAsPrefab{
                  .name = name,
                  .entity = ctx.selectionCtx->selectedEntity,
              }
          );

          saveAsPrefabNameBuffer_[0] = '\0';
          ImGui::CloseCurrentPopup();
        }

        if (!isValidName)
          ImGui::EndDisabled();
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        saveAsPrefabNameBuffer_[0] = '\0';
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void EntityInspectorPanel::renderLinkToPrefabModal(const UIContext& ctx) {
    if (shouldOpenLinkToPrefabModal_) {
      ImGui::OpenPopup("Link Entity to Prefab");
      prefabSearchBuffer_[0] = '\0';
      shouldOpenLinkToPrefabModal_ = false;
    }

    if (ImGui::BeginPopupModal("Link Entity to Prefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputTextWithHint(
          "##SearchPrefabName", "Search prefab...", prefabSearchBuffer_, sizeof(prefabSearchBuffer_)
      );

      ImGui::Separator();

      if (ImGui::BeginChild("PrefabListRegion", ImVec2(350.f, 300.f), ImGuiChildFlags_Borders)) {
        std::string lowerNameFilter{ prefabSearchBuffer_ };
        std::transform(lowerNameFilter.begin(), lowerNameFilter.end(), lowerNameFilter.begin(), [](unsigned char ch) {
          return std::tolower(ch);
        });

        const auto& prefabNames{ ctx.engineCtx.prefabManager->getNames() };
        for (const auto& prefabName : prefabNames) {
          std::string lowerPrefabName{ prefabName };
          std::transform(lowerPrefabName.begin(), lowerPrefabName.end(), lowerPrefabName.begin(), [](unsigned char ch) {
            return std::tolower(ch);
          });

          if (!lowerNameFilter.empty() && lowerPrefabName.find(lowerNameFilter) == std::string::npos)
            continue;

          const bool isSelected{ selectedPrefabName_ == prefabName };
          if (ImGui::Selectable(prefabName.c_str(), isSelected)) {
            selectedPrefabName_ = prefabName;
          }

          if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            ctx.engineCtx.eventQueue->publish(
                event::RequestLinkEntityToPrefab{
                    .entity = ctx.selectionCtx->selectedEntity,
                    .handle = ctx.engineCtx.prefabManager->getHandle(prefabName),
                }
            );
            selectedPrefabName_ = "";
            ImGui::CloseCurrentPopup();
          }
        }
      }
      ImGui::EndChild();

      const bool hasPrefabSelected{ !selectedPrefabName_.empty() };

      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      {  // Link Button
        if (!hasPrefabSelected)
          ImGui::BeginDisabled();

        if (ImGui::Button("Link", ImVec2(buttonWidth, 0.0f))) {
          ctx.engineCtx.eventQueue->publish(
              event::RequestLinkEntityToPrefab{
                  .entity = ctx.selectionCtx->selectedEntity,
                  .handle = ctx.engineCtx.prefabManager->getHandle(selectedPrefabName_),
              }
          );
          selectedPrefabName_ = "";
          ImGui::CloseCurrentPopup();
        }

        if (!hasPrefabSelected)
          ImGui::EndDisabled();
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        selectedPrefabName_ = "";
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void EntityInspectorPanel::renderSyncFromPrefabModal(const UIContext& ctx) {
    if (shouldOpenSyncFromPrefabModal_) {
      ImGui::OpenPopup("Sync Entity from Prefab");
      shouldOpenSyncFromPrefabModal_ = false;
    }

    if (ImGui::BeginPopupModal("Sync Entity from Prefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::SetNextItemWidth(-1.f);
      ImGui::InputTextWithHint(
          "##SearchComponent", "Search component...", syncComponentFilter_, sizeof(syncComponentFilter_)
      );
      ImGui::Separator();

      ImGui::BeginChild("ComponentListRegion", ImVec2(350.0f, 300.0f), ImGuiChildFlags_Borders);

      std::string filterLower{ syncComponentFilter_ };
      std::transform(filterLower.begin(), filterLower.end(), filterLower.begin(), [](unsigned char ch) {
        return std::tolower(ch);
      });

      auto prefabInstance{
        ctx.sceneCtx.registry->getComponent<component::PrefabInstance>(ctx.selectionCtx->selectedEntity)
      };
      auto prefabInfo{ ctx.engineCtx.prefabManager->getPrefabInfo(prefabInstance.handle) };

      for (auto componentTypeId : prefabInfo->componentTypeIds) {
        auto componentType{ entt::resolve(componentTypeId) };
        const reflection_system::ComponentInfo* componentInfo{ componentType.custom() };

        if (componentInfo && !componentInfo->isComponent)
          continue;

        auto* set{ ctx.sceneCtx.registry->getISparseSetByTypeId(componentTypeId) };

        if (!set)
          continue;

        bool hasEntityComponent{ set->hasComponent(ctx.selectionCtx->selectedEntity) };

        const char* typeName{ componentInfo->displayName ? componentInfo->displayName
                              : componentType.name()     ? componentType.name()
                                                         : "Unknown" };
        std::string nameLower{ typeName };
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), [](unsigned char ch) {
          return std::tolower(ch);
        });

        if (!filterLower.empty() && nameLower.find(filterLower) == std::string::npos) {
          continue;
        }

        auto it{ std::find(selectedComponentsToSync_.begin(), selectedComponentsToSync_.end(), componentTypeId) };
        bool isSelected{ it != selectedComponentsToSync_.end() };

        std::string label{ typeName };
        if (!hasEntityComponent)
          label += " [new]";

        if (ImGui::Checkbox(label.c_str(), &isSelected)) {
          if (isSelected) {
            selectedComponentsToSync_.push_back(componentTypeId);
          } else {
            selectedComponentsToSync_.erase(it);
          }
        }
      }
      ImGui::EndChild();

      ImGui::Separator();

      const bool hasComponentsSelected{ !selectedComponentsToSync_.empty() };
      const float availWidth{ ImGui::GetContentRegionAvail().x };
      const float buttonWidth{ availWidth * 0.25f };
      const float spacing{ ImGui::GetStyle().ItemSpacing.x };
      const float totalButtonWidth{ 2.f * buttonWidth + spacing };

      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availWidth - totalButtonWidth);

      {  // Add button
        if (!hasComponentsSelected)
          ImGui::BeginDisabled();

        if (ImGui::Button("Sync", ImVec2(buttonWidth, 0.0f))) {
          for (auto typeId : selectedComponentsToSync_) {
            ctx.engineCtx.eventQueue->publish(
                event::RequestSyncComponentFromPrefab{
                    .entity = ctx.selectionCtx->selectedEntity,
                    .typeId = typeId,
                }
            );
          }
          selectedComponentsToSync_.clear();
          syncComponentFilter_[0] = '\0';
          ImGui::CloseCurrentPopup();
        }

        if (!hasComponentsSelected)
          ImGui::EndDisabled();
      }

      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f))) {
        selectedComponentsToSync_.clear();
        syncComponentFilter_[0] = '\0';
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  bool EntityInspectorPanel::filterComponent(const std::string& componentName) const {
    if (lowerFilterBuffer_.empty())
      return true;

    std::string lowerName{ componentName };
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char ch) {
      return std::tolower(ch);
    });

    return lowerName.find(lowerFilterBuffer_) != std::string::npos;
  }

  bool EntityInspectorPanel::filterProperty(const reflection_system::PropertyInfo* propInfo) const {
    if (!propInfo)
      return true;

    if (hideReadOnly_ && propInfo->readOnly)
      return false;

    if (hideTransient_ && propInfo->transient)
      return false;

    return true;
  }

}  // namespace ls::ui
