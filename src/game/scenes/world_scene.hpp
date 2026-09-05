#pragma once

#include "engine/core/i_scene.hpp"
#include "engine/core/scene_context.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/input/input_context.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/ui/ui_manager.hpp"
#include "game/items/item_registry.hpp"
#include "game/scenes/scene_names.hpp"

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
