#include "engine/ui/ui_system.hpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include "engine/renderer/renderer_system.hpp"

namespace ls::ui_system {

  namespace {
    bool isImGuiInit{ false };
  }

  void init(SDL_Window* window, SDL_GLContext glContext) {
    assert(!isImGuiInit && "ImGui is already initialized");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() };
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 460");
    isImGuiInit = true;
  }

  void shutdown() {
    assert(isImGuiInit && "Attempted to shutdown non-initialized ImGui.");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    isImGuiInit = false;
  }

  void processEvent(const SDL_Event* event) { ImGui_ImplSDL3_ProcessEvent(event); }

  void beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
  }

  void endFrame() {
    renderer_system::bindFramebuffer(0);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

}  // namespace ls::ui_system
