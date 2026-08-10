#include "engine/ui/panels/entity_explorer_panel.hpp"

#include <imgui.h>

#include <entt/core/fwd.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <glm/glm.hpp>
#include <string>

#include "engine/components/entity_name.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
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
      const ls::reflection::PropertyInfo* propInfo{ data.custom() };
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

      if (value.type() == entt::resolve<bool>()) {
        bool val{ value.cast<bool>() };
        if (ImGui::Checkbox(label, &val)) {
          value = val;
          valueChanged = true;
        }
      } else if (value.type() == entt::resolve<float>()) {
        float val{ value.cast<float>() };
        if (ImGui::DragFloat(label, &val, 0.05f)) {
          value = val;
          valueChanged = true;
        }
      } else if (value.type() == entt::resolve<int>()) {
        int val{ value.cast<int>() };
        if (ImGui::DragInt(label, &val, 1)) {
          value = val;
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
      } else if (auto valueType = value.type(); valueType.data().begin() != valueType.data().end()) {
        if (ImGui::TreeNode(label)) {
          ImGui::PushID(static_cast<int>(1));

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

  void EntityExplorerPanel::render(const UIContext& ctx) {
    if (!ctx.registry || !ctx.textureManager)
      return;

    if (!ctx.registry->isValidEntity(selectedEntity_))
      selectedEntity_ = ecs::kNullEntity;

    if (ImGui::Begin("Entity Explorer", &visible_)) {
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

          for (ecs::EntityId entity{ 0 }; entity < ctx.registry->getMaxEntityId(); entity++) {
            if (!filterEntity(*ctx.registry, entity))
              continue;

            {  // Entity
              ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen };
              if (entity == selectedEntity_) {
                flags |= ImGuiTreeNodeFlags_Selected;
              }

              std::string label{ getEntityLabel(*ctx.registry, entity) };
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

        if (ctx.registry->isValidEntity(selectedEntity_)) {
          ImGui::PushItemWidth(120.f);

          inspectEntity(*ctx.registry, selectedEntity_);

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

  bool EntityExplorerPanel::filterEntity(const ecs::Registry& registry, const ecs::EntityId entity) {
    if (!registry.isValidEntity(entity))
      return false;

    if (hideNameless_) {
      if (!registry.hasComponent<component::EntityName>(entity))
        return false;
    }

    return true;
  }

}  // namespace ls::ui
