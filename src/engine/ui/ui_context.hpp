#pragma once

#include "engine/core/engine_context.hpp"
#include "engine/core/scene_context.hpp"
#include "engine/ui/selection_context.hpp"
#include "engine/ui/ui_style.hpp"

namespace ls::ui {

  struct UIContext {
    EngineContext engineCtx;
    SceneContext sceneCtx;
    SelectionContext* selectionCtx{ nullptr };
    UIStyle* uiStyle{ nullptr };
  };

}  // namespace ls::ui
