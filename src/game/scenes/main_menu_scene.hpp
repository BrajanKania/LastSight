#pragma once

#include <glm/ext/vector_float2.hpp>
#include <vector>

#include "engine/core/i_scene.hpp"
#include "engine/dispatch/event_queue.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/ecs/types.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/ui/ui_manager.hpp"
#include "game/components/field_of_view.hpp"
#include "game/components/menu_stalker_point.hpp"

namespace ls {

  class MainMenuScene : public IScene {
  public:
    explicit MainMenuScene(dispatch::EventQueue& engineEventQueue)
        : engineEventQueue_{ engineEventQueue } {}

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
        .renderPipeline = &renderPipeline_,
        .textureManager = &textureManager_,
      };
    }

  private:
    ui::UIContext getUIContext() override {
      return ui::UIContext{
        .eventQueue = engineEventQueue_,
        .sceneCtx = getSceneContext(),
      };
    }

    void updateMenuStalker(const component::FieldOfView& fov, glm::vec2 mouseWorldPos, float dt);

    ecs::Registry registry_;
    gfx::TextureManager textureManager_;
    ui::UIManager uiManager_;
    input::InputManager inputManager_;

    renderer::RenderPipeline renderPipeline_;
    std::shared_ptr<gfx::Framebuffer> worldFBO_{ nullptr };
    std::shared_ptr<gfx::Framebuffer> fovFBO_{ nullptr };

    dispatch::EventQueue eventQueue_;
    dispatch::EventQueue& engineEventQueue_;

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
