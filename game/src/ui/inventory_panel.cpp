#include "ui/inventory_panel.hpp"

#include <imgui.h>

#include "components/inventory.hpp"
#include "components/player.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/gfx/texture_manager.hpp"
#include "ls/input/input_system.hpp"
#include "ui/panel_names.hpp"

namespace ls::ui {

  InventoryPanel::InventoryPanel(const item::ItemRegistry& itemRegistry)
      : itemRegistry_{ itemRegistry } {
    visible_ = true;
  }

  void InventoryPanel::render(const UIContext& ctx) {
    if (!ctx.sceneCtx.registry || !ctx.engineCtx.textureManager) {
      return;
    }

    for (auto entity : ctx.sceneCtx.registry->view<component::Player, component::Inventory>()) {
      const auto& inventory{ ctx.sceneCtx.registry->getComponent<component::Inventory>(entity) };

      glm::vec2 vpPosition{ input_system::getViewportPosition() };
      glm::vec2 vpSize{ input_system::getViewportSize() };

      ImVec2 windowPos(vpPosition.x + 10.f, vpPosition.y + vpSize.y * 0.5f);
      ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(0.f, 0.5f));

      ImGuiWindowFlags flags{ ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                              ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground };

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 8.f));

      if (ImGui::Begin(ui::panel::kInventory, nullptr, flags)) {
        for (std::size_t i{ 0 }; i < inventory.slots.size(); ++i) {
          bool isActive{ i == inventory.activeSlotIndex };
          const auto& slot{ inventory.slots[i] };

          ImVec4 borderColor{ isActive ? ImVec4(0.8f, 0.8f, 0.8f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 0.6f) };
          ImVec4 bgColor{ isActive ? ImVec4(0.4f, 0.45f, 0.45f, 0.8f) : ImVec4(0.3f, 0.3f, 0.3f, 0.5f) };

          ImGui::PushStyleColor(ImGuiCol_Border, borderColor);
          ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor);
          ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 16.f);
          ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2);

          std::string childId{ "slot_" + std::to_string(i) };
          ImGui::BeginChild(childId.c_str(), ImVec2(60.f, 60.f), true, ImGuiWindowFlags_NoScrollbar);

          const auto* itemDef{ itemRegistry_.get(slot.itemId) };
          if (itemDef && itemDef->iconTextureHandle) {
            const auto* texture{ ctx.engineCtx.textureManager->get(itemDef->iconTextureHandle) };
            if (texture) {
              ImTextureID textureHandle{ (ImTextureID)(uintptr_t)texture->getId() };
              ImGui::Image(textureHandle, ImVec2(40.f, 40.f));
            }
          }

          ImGui::EndChild();
          ImGui::PopStyleVar(2);
          ImGui::PopStyleColor(2);
        }
      }
      ImGui::End();

      ImGui::PopStyleVar();
    }
  }

}  // namespace ls::ui
