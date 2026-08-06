#include "game/ui/inventory_panel.hpp"

#include <imgui.h>

#include "game/components/inventory.hpp"
#include "game/components/player.hpp"

namespace ls::ui {

  InventoryPanel::InventoryPanel(const item::ItemRegistry& itemRegistry)
      : itemRegistry_{ itemRegistry } {
    visible_ = true;
  }

  void InventoryPanel::render(const UIContext& ctx) {
    for (auto entity : ctx.registry.view<component::Player, component::Inventory>()) {
      const auto& inventory{ ctx.registry.getComponent<component::Inventory>(entity) };

      ImGuiIO& io{ ImGui::GetIO() };
      ImVec2 windowPos(10.f, io.DisplaySize.y * 0.5f);
      ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(0.f, 0.5f));

      ImGuiWindowFlags flags{ ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                              ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground };

      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 8.f));

      if (ImGui::Begin("##InventoryHUD", nullptr, flags)) {
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
          if (itemDef && itemDef->iconTextureId != 0) {
            const auto* texture{ ctx.textureManager.get(itemDef->iconTextureId) };
            if (texture) {
              ImTextureID texHandle{ (ImTextureID)(uintptr_t)texture->getId() };
              ImGui::Image(texHandle, ImVec2(40.f, 40.f));
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
