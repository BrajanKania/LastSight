#pragma once

#include <glm/ext/vector_float2.hpp>
#include <vector>

#include "engine/core/engine_context.hpp"
#include "engine/core/i_scene.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/ui/ui_manager.hpp"
#include "game/components/field_of_view.hpp"
#include "game/components/menu_stalker_point.hpp"
#include "game/scenes/scene_names.hpp"

namespace ls {

  class MainMenuScene : public IScene {
  public:
    explicit MainMenuScene(EngineContext engineCtx)
        : IScene(engineCtx) {}

    void onEnter() override;
    void onExit() override;
    void onResize(int width, int height) override;

    void handleInput(bool blockKeyboard, bool blockMouse) override;
    void update(float dt) override;
    void render() override;
    void renderUI() override;

    bool isOpaque() const override { return true; }

    SceneContext getSceneContext() override {
      return SceneContext{
        .registry = &registry_,
        .eventQueue = &eventQueue_,
        .renderPipeline = &renderPipeline_,
        .uiManager = &uiManager_,
        .sceneFBO = fovFBO_,
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

    renderer::RenderPipeline renderPipeline_;
    std::shared_ptr<gfx::Framebuffer> worldFBO_{ nullptr };
    std::shared_ptr<gfx::Framebuffer> fovFBO_{ nullptr };

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
