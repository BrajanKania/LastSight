#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

#include "ls/ui/ui_style.hpp"

namespace ls::ui_system {

  void init(SDL_Window* window, SDL_GLContext glContext);

  void changeUIStyle(ui::UIStyle uiStyle);

  void shutdown();

  void processEvent(const SDL_Event* event);

  void beginFrame();

  void endFrame();

}  // namespace ls::ui_system
