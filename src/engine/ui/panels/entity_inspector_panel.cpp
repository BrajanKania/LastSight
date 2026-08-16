#include "engine/ui/panels/entity_inspector_panel.hpp"

#include <imgui.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <entt/core/fwd.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <format>
#include <glm/glm.hpp>
#include <string>

#include "engine/components/entity_name.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/reflection/reflection_system.hpp"
#include "engine/ui/panels/panel_names.hpp"
#include "engine/ui/ui_context.hpp"

namespace ls::ui {

  namespace {

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
    assert(ctx.selectionCtx != nullptr && "[EntityInspectorPanel] Requires a valid SelectionContext!");
    assert(ctx.sceneCtx.registry != nullptr && "[EntityInspectorPanel] Requires a valid Registry!");
    assert(ctx.engineCtx.textureManager != nullptr && "[EntityInspectorPanel] Requires a valid TextureManager!");

    if (!ctx.sceneCtx.registry->isValidEntity(ctx.selectionCtx->selectedEntity)) {
      ctx.selectionCtx->selectedEntity = ecs::kNullEntity;
    }

    if (ImGui::Begin(ui::panel::kEntityInspector, &visible_)) {
      {  // Inspector

        ImGui::BeginChild("InspectorRegion");

        if (ctx.sceneCtx.registry->isValidEntity(ctx.selectionCtx->selectedEntity)) {
          ImGui::PushItemWidth(120.f);

          inspectEntity(*ctx.sceneCtx.registry, ctx.selectionCtx->selectedEntity);

          ImGui::PopItemWidth();
        } else {
          ImGui::Text("Select an entity from the list");
        }

        ImGui::EndChild();
      }
    }
    ImGui::End();
  }

}  // namespace ls::ui
