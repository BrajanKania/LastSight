#pragma once
#include <memory>

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
    class Framebuffer;
  }

  struct SceneContext {
    ecs::Registry* registry{ nullptr };
    dispatch::EventQueue* eventQueue{ nullptr };
    input::InputManager* inputManager{ nullptr };
    renderer::RenderPipeline* renderPipeline{ nullptr };
    std::shared_ptr<gfx::Framebuffer> sceneFBO{ nullptr };
  };

}  // namespace ls
