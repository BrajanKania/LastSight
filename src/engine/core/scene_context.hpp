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
  namespace renderer {
    class RenderPipeline;
  }
  namespace gfx {
    class TextureManager;
  }

  struct SceneContext {
    ecs::Registry* registry{ nullptr };
    dispatch::EventQueue* eventQueue{ nullptr };
    input::InputManager* inputManager{ nullptr };
    renderer::RenderPipeline* renderPipeline{ nullptr };
    gfx::TextureManager* textureManager{ nullptr };
  };

}  // namespace ls
