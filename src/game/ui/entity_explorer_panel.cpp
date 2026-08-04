#include "game/ui/entity_explorer_panel.hpp"

#include <imgui.h>

#include <cfloat>
#include <string>

#include "engine/components/entity_name.hpp"
#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/ui/ui_context.hpp"
#include "game/components/camera.hpp"

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

    void inspectComponentTransform(ecs::Registry& registry, const ecs::EntityId entity) {
      if (registry.hasComponent<component::Transform>(entity)) {
        if (ImGui::CollapsingHeader("Transform")) {
          auto& transform{ registry.getComponent<component::Transform>(entity) };
          ImGui::DragFloat2("Position", &transform.position.x, 0.05f);
          ImGui::DragFloat2("Scale", &transform.scale.x, 0.05f, 0.f);
          ImGui::DragFloat("Rotation", &transform.rotation, 1.0f, -180.f, 180.f);
        }
      }
    }

    void inspectComponentSprite(
        ecs::Registry& registry, gfx::TextureManager& textureManager, const ecs::EntityId entity
    ) {
      if (registry.hasComponent<component::Sprite>(entity)) {
        if (ImGui::CollapsingHeader("Sprite")) {
          auto& sprite{ registry.getComponent<component::Sprite>(entity) };
          ImGui::ColorEdit4("Color", &sprite.color.r);
          ImGui::DragFloat2("UV Scale", &sprite.uvScale.s, 0.05f, 0.f, FLT_MAX);
          int textureId{ static_cast<int>(sprite.textureId) };
          if (ImGui::DragInt("Texture Id", &textureId, 0.05f, 0, textureManager.getTextureCount())) {
            sprite.textureId = textureId;
          }
          ImGui::DragFloat("Angle Offset", &sprite.angleOffset, 1.f, -180.f, 180.f);
        }
      }
    }

    void inspectComponentCamera(ecs::Registry& registry, const ecs::EntityId entity) {
      if (registry.hasComponent<component::Camera>(entity)) {
        if (ImGui::CollapsingHeader("Camera")) {
          auto& camera{ registry.getComponent<component::Camera>(entity) };
          ImGui::DragFloat("Orthographic size", &camera.orthographicSize, 0.05f, 0.f, FLT_MAX);
          ImGui::DragFloat("Zoom", &camera.zoom, 0.05f, 0.f, FLT_MAX);
        }
      }
    }

  }  // namespace

  void EntityExplorerPanel::render(const UIContext& ctx) {
    if (!ctx.registry.isValidEntity(selectedEntity_))
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

          for (ecs::EntityId entity{ 0 }; entity < ctx.registry.getMaxEntityId(); entity++) {
            if (!filterEntity(ctx.registry, entity))
              continue;

            {  // Entity
              ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen };
              if (entity == selectedEntity_) {
                flags |= ImGuiTreeNodeFlags_Selected;
              }

              std::string label{ getEntityLabel(ctx.registry, entity) };
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

        if (ctx.registry.isValidEntity(selectedEntity_)) {
          ImGui::PushItemWidth(120.f);

          inspectComponentTransform(ctx.registry, selectedEntity_);
          inspectComponentSprite(ctx.registry, ctx.textureManager, selectedEntity_);
          inspectComponentCamera(ctx.registry, selectedEntity_);

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
