#include "engine/ui/panels/scene_hierarchy_panel.hpp"

#include <imgui.h>

#include <cassert>

#include "engine/components/entity_name.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/events/request_save_scene.hpp"
#include "engine/ui/panels/panel_names.hpp"

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
  }  // namespace

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

    if (ImGui::Begin(ui::panel::kSceneHierarchy, &visible_)) {
      {  // Save
        if (ImGui::Button("Save scene")) {
          ctx.sceneCtx.eventQueue->publish(event::RequestSaveScene{});
        }
      }

      {  // Filter
        ImGui::SeparatorText("Filter");
        ImGui::Checkbox("With name", &hideNameless_);
      }

      {  // Entity List
        ImGui::BeginChild("EntityListRegion");

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

            if (ImGui::IsItemClicked()) {
              ctx.selectionCtx->selectedEntity = entity;
            }
          }
        }

        ImGui::EndChild();
      }
    }
    ImGui::End();
  }

  bool SceneHierarchyPanel::filterEntity(const ecs::Registry& registry, const ecs::EntityId entity) const {
    if (!registry.isValidEntity(entity))
      return false;

    if (hideNameless_) {
      if (!registry.hasComponent<component::EntityName>(entity))
        return false;
    }

    return true;
  }
}  // namespace ls::ui
