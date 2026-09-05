#pragma once

namespace ls {

  namespace ecs {
    class Registry;
  }
  namespace dispatch {
    class EventQueue;
  }
  namespace input {
    class InputManager;
  }
  namespace ui {
    class UIManager;
  }

  struct SceneContext {
    ecs::Registry* registry{ nullptr };
    dispatch::EventQueue* eventQueue{ nullptr };
    input::InputManager* inputManager{ nullptr };
    ui::UIManager* uiManager{ nullptr };
  };

}  // namespace ls
