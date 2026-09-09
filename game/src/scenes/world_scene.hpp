#pragma once

#include "items/item_registry.hpp"
#include "ls/core/i_scene.hpp"
#include "ls/core/scene_context.hpp"
#include "ls/dispatch/event_queue.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/ecs/types.hpp"
#include "ls/input/input_context.hpp"
#include "ls/input/input_manager.hpp"
#include "ls/ui/ui_manager.hpp"
#include "scenes/scene_names.hpp"

namespace ls {

  class WorldScene : public IScene {
  public:
    explicit WorldScene(EngineContext engineCtx)
        : IScene(engineCtx) {}

    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void handleInput(input::InputContext& inputCtx) override;
    void update(float dt) override;
    void render() override;
    void renderUI() override;

    bool isOpaque() const override { return true; }

    SceneContext getSceneContext() override {
      return SceneContext{
        .registry = &registry_,
        .eventQueue = &eventQueue_,
        .inputManager = &inputManager_,
        .uiManager = &uiManager_,
      };
    }

    const char* getName() const override { return scene::kWorld; }

  private:
    void processEvents() override;
    void genereteEntities();

    ecs::Registry registry_;
    dispatch::EventQueue eventQueue_;
    ui::UIManager uiManager_;
    input::InputManager inputManager_;

    item::ItemRegistry itemRegistry_;

    ecs::EntityId player_{ 0 };
  };

}  // namespace ls
