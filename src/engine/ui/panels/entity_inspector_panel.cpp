#include "engine/ui/panels/entity_inspector_panel.hpp"

#include <imgui.h>

#include <cstdint>
#include <cstring>
#include <entt/core/fwd.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <format>
#include <glm/glm.hpp>
#include <string>

#include "engine/components/entity_name.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/events/request_save_scene.hpp"
#include "engine/reflection/reflection_system.hpp"
#include "engine/ui/ui_context.hpp"

namespace ls::ui {

  namespace {

    std::string getEntityLabel(ecs::Registry& registry, const ecs::EntityId entity) {
      std::string label{ std::format("[{}]", entity) };

      if (registry.hasComponent<component::EntityName>(entity)) {
        const std::string& name{ registry.getComponent<component::EntityName>(entity).name };
        label += std::format(" {}", name);
      }

      return label;
    }

    bool inspectComponentProperty(entt::meta_any& owner, entt::meta_data data) {
      const ls::reflection_system::PropertyInfo* propInfo{ data.custom() };
      const bool isReadOnly{ propInfo ? propInfo->readOnly : false };
      const char* label{ (propInfo && propInfo->displayName) ? propInfo->displayName
                                                             : (data.name() ? data.name() : "Unassigned") };

      entt::meta_any value{ data.get(owner) };

      if (!value)
        return false;

      bool valueChanged{ false };

      if (isReadOnly) {
        ImGui::BeginDisabled(true);
      }

      auto valueType{ value.type() };

      if (valueType == entt::resolve<bool>()) {
        bool val{ value.cast<bool>() };
        if (ImGui::Checkbox(label, &val)) {
          value = val;
          valueChanged = true;
        }
      } else if (valueType == entt::resolve<float>()) {
        float val{ value.cast<float>() };
        if (ImGui::DragFloat(label, &val, 0.05f)) {
          value = val;
          valueChanged = true;
        }
      } else if (valueType == entt::resolve<int>()) {
        int val{ value.cast<int>() };
        if (ImGui::DragInt(label, &val, 1)) {
          value = val;
          valueChanged = true;
        }
      } else if (valueType == entt::resolve<uint32_t>()) {
        uint32_t val{ value.cast<uint32_t>() };
        if (ImGui::DragScalar(label, ImGuiDataType_U32, &val, 1.0f)) {
          value = val;
          valueChanged = true;
        }
      } else if (valueType == entt::resolve<std::size_t>()) {
        std::size_t val{ value.cast<std::size_t>() };
        if (ImGui::DragScalar(label, ImGuiDataType_U64, &val, 1.0f)) {
          value = val;
          valueChanged = true;
        }
      } else if (auto* str{ value.try_cast<std::string>() }) {
        char buffer[256];
        std::strncpy(buffer, str->c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';
        if (ImGui::InputText(label, buffer, sizeof(buffer))) {
          value = std::string(buffer);
          valueChanged = true;
        }
      } else if (auto* vec{ value.try_cast<glm::vec2>() }) {
        if (ImGui::DragFloat2(label, &vec->x, 0.05f)) {
          valueChanged = true;
        }
      } else if (auto* vec{ value.try_cast<glm::vec3>() }) {
        if (ImGui::DragFloat3(label, &vec->x, 0.05f)) {
          valueChanged = true;
        }
      } else if (auto* vec{ value.try_cast<glm::vec4>() }) {
        if (ImGui::DragFloat4(label, &vec->x, 0.05f)) {
          valueChanged = true;
        }
      } else if (valueType.is_enum()) {
        std::string currentEnumName{ "Unassigned" };

        for (auto [subDataId, enumData] : valueType.data()) {
          entt::meta_any enumVal{ enumData.get({}) };
          if (enumVal == value) {
            currentEnumName = enumData.name() ? enumData.name() : "Unassigned";
            break;
          }
        }

        if (ImGui::BeginCombo(label, currentEnumName.c_str())) {
          for (auto [subDataId, enumData] : valueType.data()) {
            entt::meta_any enumVal{ enumData.get({}) };
            const char* itemName{ enumData.name() ? enumData.name() : "Unassigned" };
            const bool isSelected{ enumVal == value };

            if (ImGui::Selectable(itemName, isSelected)) {
              value = enumVal;
              valueChanged = true;
            }

            if (isSelected) {
              ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }
      } else if (valueType.data().begin() != valueType.data().end()) {
        if (ImGui::TreeNode(label)) {
          ImGui::PushID(label);

          for (auto [subDataId, subData] : valueType.data()) {
            if (inspectComponentProperty(value, subData)) {
              valueChanged = true;
            }
          }

          ImGui::PopID();
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

    void inspectEntity(ecs::Registry& registry, const ecs::EntityId entity) {
      for (auto [id, type] : entt::resolve()) {
        auto* set{ registry.getISparseSetByTypeId(id) };
        if (!set)
          continue;

        void* rawComponent{ set->getRawComponent(entity) };
        if (!rawComponent)
          continue;

        entt::meta_any anyComponent{ type.from_void(rawComponent) };
        const char* headerName{ type.name() ? type.name() : "Unassigned" };

        if (ImGui::CollapsingHeader(headerName)) {
          ImGui::PushID(static_cast<int>(id));

          for (auto [dataId, data] : type.data()) {
            inspectComponentProperty(anyComponent, data);
          }

          ImGui::PopID();
        }
      }
    }

  }  // namespace

  void EntityInspectorPanel::render(const UIContext& ctx) {
    if (!ctx.sceneCtx.registry || !ctx.engineCtx.textureManager) {
      hideNameless_ = true;
      selectedEntity_ = ecs::kNullEntity;
      return;
    }

    if (!ctx.sceneCtx.registry->isValidEntity(selectedEntity_))
      selectedEntity_ = ecs::kNullEntity;

    if (ImGui::Begin("Entity Inspector", &visible_)) {
      if (ImGui::Button("Save scene")) {
        ctx.sceneCtx.eventQueue->publish(event::RequestSaveScene{});
      }

      ImGui::Columns(2, "ExplorerSpliter", true);

      {  // Entities
        ImGui::Text("Entities");

        {  // Filter
          ImGui::SeparatorText("Filter");
          ImGui::Checkbox("With name", &hideNameless_);
        }

        {  // Entity Tree
          ImGui::BeginChild("EntityListRegion");

          ImGui::SeparatorText("Entities");

          for (ecs::EntityId entity{ 0 }; entity < ctx.sceneCtx.registry->getMaxEntityId(); entity++) {
            if (!filterEntity(*ctx.sceneCtx.registry, entity))
              continue;

            {  // Entity
              ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen };
              if (entity == selectedEntity_) {
                flags |= ImGuiTreeNodeFlags_Selected;
              }

              std::string label{ getEntityLabel(*ctx.sceneCtx.registry, entity) };
              ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uintptr_t>(entity)), flags, "%s", label.c_str());

              if (ImGui::IsItemClicked()) {
                selectedEntity_ = entity;
              }
            }
          }

          ImGui::EndChild();
        }
      }

      ImGui::NextColumn();

      {  // Inspector
        ImGui::Text("Inspector");
        ImGui::Separator();

        ImGui::BeginChild("InspectorRegion");

        if (ctx.sceneCtx.registry->isValidEntity(selectedEntity_)) {
          ImGui::PushItemWidth(120.f);

          inspectEntity(*ctx.sceneCtx.registry, selectedEntity_);

          ImGui::PopItemWidth();
        } else {
          ImGui::Text("Select an entity from the list");
        }

        ImGui::EndChild();
      }

      ImGui::Columns(1);
    }
    ImGui::End();
  }

  bool EntityInspectorPanel::filterEntity(const ecs::Registry& registry, const ecs::EntityId entity) {
    if (!registry.isValidEntity(entity))
      return false;

    if (hideNameless_) {
      if (!registry.hasComponent<component::EntityName>(entity))
        return false;
    }

    return true;
  }

}  // namespace ls::ui
