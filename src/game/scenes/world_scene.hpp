#pragma once

#include <memory>

#include "engine/core/i_scene.hpp"
#include "engine/core/scene_context.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/gfx/framebuffer.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/ui/ui_manager.hpp"
#include "game/items/item_registry.hpp"

namespace ls {

  class WorldScene : public IScene {
  public:
    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void handleInput() override;
    void update(float dt) override;
    void render() override;

    bool isOpaque() const override { return true; }

    SceneContext getSceneContext() override {
      return SceneContext{
        .registry = &registry_,
        .eventQueue = &eventQueue_,
        .inputManager = &inputManager_,
        .renderPipeline = &renderPipeline_,
        .textureManager = &textureManager_,
      };
    }

  private:
    ui::UIContext getUIContext() override {
      return ui::UIContext{
        .eventQueue = eventQueue_,
        .sceneCtx = getSceneContext(),
      };
    }

    ecs::Registry registry_;
    dispatch::EventQueue eventQueue_;
    ui::UIManager uiManager_;
    gfx::TextureManager textureManager_;
    input::InputManager inputManager_;
    renderer::RenderPipeline renderPipeline_;

    item::ItemRegistry itemRegistry_;

    std::shared_ptr<gfx::Framebuffer> worldFBO_{ nullptr };
    std::shared_ptr<gfx::Framebuffer> fovFBO_{ nullptr };
    std::shared_ptr<gfx::Framebuffer> processedFBO_{ nullptr };

    ecs::EntityId player_{ 0 };
  };

}  // namespace ls
