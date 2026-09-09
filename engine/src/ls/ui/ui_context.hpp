#pragma once

#include "ls/core/engine_context.hpp"
#include "ls/core/scene_context.hpp"
#include "ls/ui/editor_preferences.hpp"
#include "ls/ui/selection_context.hpp"
#include "ls/ui/ui_style.hpp"

namespace ls::ui {

  struct UIContext {
    EngineContext engineCtx;
    SceneContext sceneCtx;
    SelectionContext* selectionCtx{ nullptr };
    UIStyle* uiStyle{ nullptr };
    EditorPreferences* editorPreferences{ nullptr };
  };

}  // namespace ls::ui
