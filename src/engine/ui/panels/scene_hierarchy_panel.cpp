#include "engine/ui/panels/scene_hierarchy_panel.hpp"

#include <imgui.h>

#include <cassert>
#include <cctype>
#include <format>
#include <string>

#include "engine/components/entity_name.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/events/request_create_entity.hpp"
#include "engine/events/request_destroy_entity.hpp"
#include "engine/events/request_duplicate_entity.hpp"
#include "engine/events/request_save_scene.hpp"
#include "engine/ui/panels/panel_names.hpp"

namespace ls::ui {

  void SceneHierarchyPanel::render(const UIContext& ctx) {
    assert(ctx.selectionCtx != nullptr && "[SceneHierarchyPanel] Requires a valid SelectionContext!");
    assert(ctx.sceneCtx.eventQueue != nullptr && "[SceneHierarchyPanel] Requires a valid EventQueue!");

    if (!ctx.sceneCtx.registry) {
      hideNameless_ = true;
      ctx.selectionCtx->selectedEntity = ecs::kNullEntity;
      return;
    }

    if (!ctx.sceneCtx.registry->isValidEntity(ctx.selectionCtx->selectedEntity)) {
      ctx.selectionCtx->selectedEntity = ecs::kNullEntity;
    }

    const bool hasSelected{ ctx.selectionCtx->selectedEntity != ecs::kNullEntity };

    ImGuiWindowFlags flags{ ImGuiWindowFlags_MenuBar };

    if (ImGui::Begin(ui::panel::kSceneHierarchy, &visible_, flags)) {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Entity")) {
          if (ImGui::MenuItem("Create", nullptr)) {
            ctx.engineCtx.eventQueue->publish(event::RequestCreateEntity{});
          }

          if (ImGui::MenuItem("Duplicate", "Ctrl + D", false, hasSelected)) {
            ctx.engineCtx.eventQueue->publish(
                event::RequestDuplicateEntity{
                    .entityToDuplicate = ctx.selectionCtx->selectedEntity,
                }
            );
          }

          if (ImGui::MenuItem("Destroy", "Delete", false, hasSelected)) {
            ctx.engineCtx.eventQueue->publish(
                event::RequestDestroyEntity{
                    .entity = ctx.selectionCtx->selectedEntity,
                }
            );
          }
          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Save")) {
          if (ImGui::MenuItem("Save Entity", "Ctrl + S", false, hasSelected)) {
          }

          if (ImGui::MenuItem("Save Scene", "Ctrl + Shift + S")) {
            ctx.engineCtx.eventQueue->publish(event::RequestSaveScene{});
          }

          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
          ImGui::MenuItem("Hide Nameless Entities", nullptr, &hideNameless_);
          ImGui::MenuItem("Hide Entity Index", nullptr, &hideEntityIndex_);
          ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
      }

      {  // Filter
        ImGui::SetNextItemWidth(-1.f);
        ImGui::InputTextWithHint("##FilterInput", "Search entities...", filterBuffer_, sizeof(filterBuffer_));
        ImGui::Separator();
      }

      {  // Entity List
        ImGui::BeginChild("EntityListRegion");

        lowerFilterBuffer_ = filterBuffer_;
        std::transform(
            lowerFilterBuffer_.begin(), lowerFilterBuffer_.end(), lowerFilterBuffer_.begin(), [](unsigned char ch) {
              return std::tolower(ch);
            }
        );
        for (ecs::EntityId entity{ 0 }; entity < ctx.sceneCtx.registry->getMaxEntityId(); entity++) {
          if (!filterEntity(*ctx.sceneCtx.registry, entity))
            continue;

          {  // Entity
            ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen };
            if (entity == ctx.selectionCtx->selectedEntity) {
              flags |= ImGuiTreeNodeFlags_Selected;
            }

            std::string label{ getEntityLabel(*ctx.sceneCtx.registry, entity) };
            ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uintptr_t>(entity)), flags, "%s", label.c_str());

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
              ctx.selectionCtx->selectedEntity = entity;
            }

            if (ImGui::BeginPopupContextItem()) {
              ctx.selectionCtx->selectedEntity = entity;
              if (ImGui::MenuItem("Duplicate", "Ctrl + D")) {
                ctx.engineCtx.eventQueue->publish(
                    event::RequestDuplicateEntity{
                        .entityToDuplicate = ctx.selectionCtx->selectedEntity,
                    }
                );
              }

              if (ImGui::MenuItem("Destroy", "Delete")) {
                ctx.engineCtx.eventQueue->publish(
                    event::RequestDestroyEntity{
                        .entity = ctx.selectionCtx->selectedEntity,
                    }
                );
              }
              ImGui::EndPopup();
            }
          }
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
          ctx.selectionCtx->selectedEntity = ecs::kNullEntity;
        }

        if (ImGui::BeginPopupContextWindow(
                "HierarchyBlankContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems
            )) {
          if (ImGui::MenuItem("Create Entity")) {
            ctx.engineCtx.eventQueue->publish(event::RequestCreateEntity{});
          }
          ImGui::EndPopup();
        }
        ImGui::EndChild();
      }
    }
    ImGui::End();
  }

  bool SceneHierarchyPanel::filterEntity(const ecs::Registry& registry, const ecs::EntityId entity) const {
    if (!registry.isValidEntity(entity))
      return false;

    const bool hasName{ registry.hasComponent<component::EntityName>(entity) };

    if (hideNameless_ && !hasName)
      return false;

    if (!lowerFilterBuffer_.empty()) {
      const auto& name{ registry.getComponent<component::EntityName>(entity) };
      std::string lowerName{ name.name };

      std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), [](unsigned char ch) {
        return std::tolower(ch);
      });
      if (lowerName.find(lowerFilterBuffer_) == std::string::npos)
        return false;
    }

    return true;
  }

  std::string SceneHierarchyPanel::getEntityLabel(ecs::Registry& registry, const ecs::EntityId entity) {
    std::string label;

    if (!hideEntityIndex_) {
      label += std::format("[{}]", entity);
    }

    if (registry.hasComponent<component::EntityName>(entity)) {
      const std::string& name{ registry.getComponent<component::EntityName>(entity).name };
      label += (hideEntityIndex_ ? name : " " + name);
    } else if (hideEntityIndex_) {
      label = std::format("[{}]", entity);
    }

    return label;
  }
}  // namespace ls::ui
