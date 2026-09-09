#pragma once

#include <glm/ext/vector_float2.hpp>
#include <vector>

#include "components/field_of_view.hpp"
#include "components/menu_stalker_point.hpp"
#include "ls/core/engine_context.hpp"
#include "ls/core/i_scene.hpp"
#include "ls/dispatch/event_queue.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/ecs/types.hpp"
#include "ls/input/input_context.hpp"
#include "ls/input/input_manager.hpp"
#include "ls/ui/ui_manager.hpp"
#include "scenes/scene_names.hpp"

namespace ls {

  class MainMenuScene : public IScene {
  public:
    explicit MainMenuScene(EngineContext engineCtx)
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
        .uiManager = &uiManager_,
      };
    }

    const char* getName() const override { return scene::kMainMenu; }

  private:
    void processEvents() override;
    void updateMenuStalker(const component::FieldOfView& fov, glm::vec2 mouseWorldPos, float dt);
    void generateEntities();

    ecs::Registry registry_;
    ui::UIManager uiManager_;
    input::InputManager inputManager_;

    dispatch::EventQueue eventQueue_;

    ecs::EntityId backgroundEntity_{ ecs::kNullEntity };
    const glm::vec2 backgroundSize_{ 8.f, 4.f };
    const glm::vec2 maxBackgroundOffset_{ 0.5f, 0.2f };

    ecs::EntityId cameraEntity_{ ecs::kNullEntity };
    ecs::EntityId fovEntity_{ ecs::kNullEntity };

    ecs::EntityId stalkerEntity_{ ecs::kNullEntity };
    std::vector<component::MenuStalkerPoint> stalkerPoints_;
    std::size_t currentStalkerPointIndex_{ 0 };
    bool wasStalkerSeen_{ false };
  };

}  // namespace ls
