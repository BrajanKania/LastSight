#include "game/ui/main_menu_panel.hpp"

#include <imgui.h>

#include <cassert>

#include "engine/dispatch/event_queue.hpp"
#include "engine/events/request_change_scene.hpp"
#include "engine/events/request_quit_engine.hpp"
#include "game/scenes/scene_names.hpp"

namespace ls::ui {

  void MainMenuPanel::render(const UIContext& ctx) {
    assert(ctx.engineCtx.eventQueue != nullptr && "MainMenuPanel requires a valid EventQueue!");

    ImGuiIO& io{ ImGui::GetIO() };

    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f)
    );
    ImGui::SetNextWindowSize(ImVec2(300.f, 220.f));

    ImGuiWindowFlags windowFlags{ ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground };

    if (ImGui::Begin("Main Menu", nullptr, windowFlags)) {
      ImVec2 buttonSize{ 260.f, 80.f };

      ImGui::Dummy(ImVec2(0.f, 15.f));

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

      if (ImGui::Button("Play", buttonSize)) {
        ctx.engineCtx.eventQueue->publish(
            event::RequestChangeScene{
                .name = scene::kWorld,
            }
        );
      }

      ImGui::PopStyleColor(3);

      ImGui::Spacing();

      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.4f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.6f, 0.6f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));

      if (ImGui::Button("Exit", buttonSize)) {
        ctx.engineCtx.eventQueue->publish(event::RequestQuitEngine{});
      }

      ImGui::PopStyleColor(3);
    }
    ImGui::End();
  }

}  // namespace ls::ui
