#include "game/scenes/main_menu_scene.hpp"

#include <imgui.h>

#include <cassert>
#include <format>
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <vector>

#include "engine/components/camera.hpp"
#include "engine/components/entity_name.hpp"
#include "engine/components/parallax.hpp"
#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/core/random_system.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/input/input_context.hpp"
#include "engine/input/input_manager.hpp"
#include "engine/input/input_system.hpp"
#include "engine/renderer/layer.hpp"
#include "engine/renderer/material/material_manager.hpp"
#include "engine/renderer/material/material_names.hpp"
#include "engine/renderer/render_phase.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/serialization/scene_serializer.hpp"
#include "engine/systems/parallax_system.hpp"
#include "game/components/field_of_view.hpp"
#include "game/components/fov_masked.hpp"
#include "game/components/menu_stalker.hpp"
#include "game/components/menu_stalker_point.hpp"
#include "game/extraction/extraction_system.hpp"
#include "game/renderer/passes/fov_pass.hpp"
#include "game/renderer/passes/lit_pass.hpp"
#include "game/scenes/scene_names.hpp"
#include "game/scenes/texture_names.hpp"
#include "game/systems/camera_system.hpp"
#include "game/ui/main_menu_panel.hpp"
#include "game/ui/panel_names.hpp"

namespace ls {

  void MainMenuScene::onEnter() {
    assert(engineCtx_.textureManager != nullptr && "[MainMenuScene] Requires a valid TextureManager!");
    assert(engineCtx_.materialManager != nullptr && "[MainMenuScene] Requires a valid MaterialManager!");
    assert(engineCtx_.renderPipeline != nullptr && "[MainMenuScene] Requires a valid RenderPipeline!");

    const float trunkParallax{ 0.15f };
    stalkerPoints_ = std::vector<component::MenuStalkerPoint>{
      component::MenuStalkerPoint{
          .position = glm::vec2{ -0.2f, 0.35f },
          .peekOffset = glm::vec2{ 0.1f, 0.f },
          .parallaxFactor = glm::vec2{ trunkParallax },
      },
      component::MenuStalkerPoint{
          .position = glm::vec2{ -1.f, -0.1f },
          .peekOffset = glm::vec2{ -0.1f, 0.f },
          .parallaxFactor = glm::vec2{ trunkParallax },
      },
      component::MenuStalkerPoint{
          .position = glm::vec2{ 1.2f, -0.5f },
          .peekOffset = glm::vec2{ -0.1f, 0.f },
          .parallaxFactor = glm::vec2{ trunkParallax },
      },
    };

    serialization::SceneSerializer sceneSerializer(getSceneContext(), engineCtx_);
    sceneSerializer.loadScene(asset_system::scene(scene::kMainMenu));

    for (auto entity : registry_.view<component::EntityName>()) {
      const auto& name{ registry_.getComponent<component::EntityName>(entity) };
      if (name.name == "background") {
        backgroundEntity_ = entity;
      }
      if (name.name == "camera") {
        cameraEntity_ = entity;
      }
      if (name.name == "cursor_fov") {
        fovEntity_ = entity;
      }
      if (name.name == "stalker") {
        stalkerEntity_ = entity;
        if (registry_.hasComponent<component::MenuStalker>(stalkerEntity_) &&
            registry_.hasComponent<component::Transform>(stalkerEntity_)) {
          auto& stalker{ registry_.getComponent<component::MenuStalker>(stalkerEntity_) };
          auto& transform{ registry_.getComponent<component::Transform>(stalkerEntity_) };
          stalker.hidePosition = stalkerPoints_[currentStalkerPointIndex_].position;
          stalker.peekOffset = stalkerPoints_[currentStalkerPointIndex_].peekOffset;
          stalker.currentPeek = 0.f;
          transform.position = stalker.hidePosition;
        }
      }
    }

    // generateEntities();

    // for (auto entity : registry_.view<component::Sprite>()) {
    //   auto& sprite{ registry_.getComponent<component::Sprite>(entity) };
    //   sprite.materialHandle = engineCtx_.materialManager->getHandle(material_name::kLit);
    // }

    engineCtx_.renderPipeline->addPass<renderer::LitPass>(renderer::RenderPhase::MainPass);
    engineCtx_.renderPipeline->addPass<renderer::FovPass>(renderer::RenderPhase::MainPass);

    uiManager_.addPanel<ui::MainMenuPanel>(ui::panel::kMainMenu);
    uiManager_.getPanel(ui::panel::kMainMenu).setVisible(true);
  }

  void MainMenuScene::onExit() { engineCtx_.renderPipeline->clearPasses(); }

  void MainMenuScene::onResize(int width, int height) {
    float aspectRatio{ static_cast<float>(width) / static_cast<float>(height) };

    if (registry_.hasComponent<component::Transform>(backgroundEntity_)) {
      auto& transform{ registry_.getComponent<component::Transform>(backgroundEntity_) };
      transform.scale = glm::vec2{ backgroundSize_.x * aspectRatio, backgroundSize_.y };
    }
  }

  void MainMenuScene::handleInput(input::InputContext& inputCtx) { inputManager_.update(inputCtx); }

  void MainMenuScene::update(float dt) {
    processEvents();

    UpdateContext ctx{
      .registry = registry_,
      .eventQueue = eventQueue_,
      .materialManager = *engineCtx_.materialManager,
      .textureManager = *engineCtx_.textureManager,
      .inputManager = inputManager_,
      .dt = dt,
    };

    glm::vec2 viewportSize{ input_system::getViewportSize() };

    if (viewportSize.x > 0.f && viewportSize.y > 0.f) {
      glm::vec2 mousePos{ inputManager_.getMousePosition() };
      glm::vec2 center{ viewportSize * 0.5f };
      glm::vec2 normalizedMouse{ (mousePos - center) / center };

      normalizedMouse = glm::clamp(normalizedMouse, glm::vec2(-1.f), glm::vec2(1.f));
      glm::vec2 targetCamPos{ normalizedMouse.x * maxBackgroundOffset_.x, -normalizedMouse.y * maxBackgroundOffset_.y };

      const float smoothing{ 5.0f };
      if (registry_.hasComponent<component::Transform>(cameraEntity_)) {
        auto& transform{ registry_.getComponent<component::Transform>(cameraEntity_) };
        transform.position = glm::mix(transform.position, targetCamPos, dt * smoothing);
      }

      parallax_system::update(ctx, targetCamPos);
    }

    if (registry_.hasComponent<component::Camera>(cameraEntity_) &&
        registry_.hasComponent<component::FieldOfView>(fovEntity_)) {
      const auto& camera{ registry_.getComponent<component::Camera>(cameraEntity_) };
      const auto& fov{ registry_.getComponent<component::FieldOfView>(fovEntity_) };
      glm::vec2 mouseWorldPos{
        camera_system::screenToWorld(input_system::getViewportMousePosition(), input_system::getViewportSize(), camera)
      };

      if (registry_.hasComponent<component::FieldOfView>(fovEntity_)) {
        auto& transform{ registry_.getComponent<component::Transform>(fovEntity_) };
        transform.position = mouseWorldPos;
      }

      updateMenuStalker(fov, mouseWorldPos, dt);
    }

    camera_system::update(ctx);

    eventQueue_.clear();
    registry_.purgeDestroyedEntities();
  }

  void MainMenuScene::render() {
    for (auto entity : registry_.view<component::Camera>()) {
      const auto& camera{ registry_.getComponent<component::Camera>(entity) };
      engineCtx_.renderPipeline->getFrameData().setCamera(camera.view, camera.projection);
      break;
    }

    for (auto entity : registry_.view<component::Transform, component::FieldOfView>()) {
      const auto& transform{ registry_.getComponent<component::Transform>(entity) };
      const auto& fov{ registry_.getComponent<component::FieldOfView>(entity) };

      glm::vec2 fovDir{ glm::cos(glm::radians(transform.rotation)), glm::sin(glm::radians(transform.rotation)) };

      if (auto* material{ engineCtx_.materialManager->get(material_name::kLit) }) {
        material->setProperty("uFovPos", transform.position);
        material->setProperty("uFovDir", fovDir);
        material->setProperty("uFovInnerRadius", fov.innerRadius);
        material->setProperty("uFovOuterRadius", fov.outerRadius);
        material->setProperty("uFovHalfAngleRad", glm::radians(fov.fovAngle / 2.f));
        material->setProperty("uFovSmoothnessRad", glm::radians(fov.smoothnessAngle));
        material->setProperty("uFovSmoothnessDist", fov.smoothnessDistance);
      }

      if (auto* material{ engineCtx_.materialManager->get(material_name::kFov) }) {
        material->setProperty("uViewPos", transform.position);
        material->setProperty("uViewDir", fovDir);
        material->setProperty("uInnerRadius", fov.innerRadius);
        material->setProperty("uOuterRadius", fov.outerRadius);
        material->setProperty("uHalfFovRad", glm::radians(fov.fovAngle / 2.f));
        material->setProperty("uSmoothnessRad", glm::radians(fov.smoothnessAngle));
        material->setProperty("uSmoothnessDistance", fov.smoothnessDistance);
        material->setProperty("uDarkness", fov.darkness);
      }
    }

    extraction_system::extractGameRenderCommands(engineCtx_.renderPipeline->getCommandBuffer(), registry_);
  }

  void MainMenuScene::renderUI() { uiManager_.render(getUIContext()); }

  void MainMenuScene::processEvents() {}

  void MainMenuScene::updateMenuStalker(const component::FieldOfView& fov, glm::vec2 mouseWorldPos, float dt) {
    if (glm::isnan(mouseWorldPos.x) || glm::isnan(mouseWorldPos.y)) {
      return;
    }

    if (!registry_.hasComponent<component::Transform>(stalkerEntity_) ||
        !registry_.hasComponent<component::MenuStalker>(stalkerEntity_) ||
        !registry_.hasComponent<component::Parallax>(stalkerEntity_)) {
      return;
    }

    auto& stalkerTransform{ registry_.getComponent<component::Transform>(stalkerEntity_) };
    auto& stalker{ registry_.getComponent<component::MenuStalker>(stalkerEntity_) };
    auto& parallax{ registry_.getComponent<component::Parallax>(stalkerEntity_) };

    float distanceToMouse{ glm::distance(mouseWorldPos, stalker.hidePosition + stalker.peekOffset) };
    float targetPeek{
      glm::clamp((distanceToMouse - stalker.minDistance) / (stalker.maxDistance - stalker.minDistance), 0.0f, 1.0f)
    };

    stalker.currentPeek = glm::mix(stalker.currentPeek, targetPeek, dt * stalker.peekSpeed);
    parallax.basePosition = stalker.hidePosition + (stalker.peekOffset * stalker.currentPeek);

    float distanceToPhysicalStalker{ glm::distance(mouseWorldPos, stalkerTransform.position) };
    bool isCurrentlyVisible{ distanceToPhysicalStalker <= fov.outerRadius };

    if (isCurrentlyVisible && !wasStalkerSeen_) {
      wasStalkerSeen_ = true;
    } else if (wasStalkerSeen_ && (!isCurrentlyVisible || glm::abs(stalker.currentPeek) < 0.02f)) {
      std::size_t newIndex{ currentStalkerPointIndex_ };
      while (newIndex == currentStalkerPointIndex_ && stalkerPoints_.size() > 1) {
        newIndex = static_cast<std::size_t>(random_system::getRangeRandom(0, stalkerPoints_.size() - 1));
      }

      currentStalkerPointIndex_ = newIndex;
      const auto& newPoint{ stalkerPoints_[currentStalkerPointIndex_] };

      stalker.hidePosition = newPoint.position;
      stalker.peekOffset = newPoint.peekOffset;
      stalker.currentPeek = 0.0f;

      parallax.factor = newPoint.parallaxFactor;
      parallax.basePosition = newPoint.position;
      stalkerTransform.position = newPoint.position;

      wasStalkerSeen_ = false;
    }
  }

  void MainMenuScene::generateEntities() {
    {  // Entities

      {  // Camera
        cameraEntity_ = registry_.createEntity();
        registry_.addComponent(cameraEntity_, component::EntityName{ .name = "camera" });
        registry_.addComponent(
            cameraEntity_,
            component::Camera{
                .orthographicSize = 2.f,
                .zoom = 1.f,
            }
        );
        registry_.addComponent(
            cameraEntity_,
            component::Transform{
                .scale = glm::vec2(1.f),
                .position = glm::vec2(0.f),
                .rotation = 0.f,
            }
        );
      }

      {  // Background
        backgroundEntity_ = registry_.createEntity();
        registry_.addComponent(backgroundEntity_, component::EntityName{ .name = "background" });
        registry_.addComponent(
            backgroundEntity_,
            component::Transform{
                .scale = backgroundSize_,
                .position = glm::vec2(0.f),
                .rotation = 0.f,
            }
        );
        registry_.addComponent(
            backgroundEntity_,
            component::Sprite{
                .color = glm::vec4(1.f),
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kMainMenuBackground),
                .layer = renderer::Layer::Background,
            }
        );
      }

      {  // Stalker
        stalkerEntity_ = registry_.createEntity();
        registry_.addComponent(stalkerEntity_, component::EntityName{ .name = "stalker" });
        registry_.addComponent(
            stalkerEntity_,
            component::Transform{
                .scale = glm::vec2(0.2f, 0.4f),
                .position = stalkerPoints_[currentStalkerPointIndex_].position,
                .rotation = 0.f,
            }
        );
        registry_.addComponent(
            stalkerEntity_,
            component::Sprite{
                .color = glm::vec4(1.f),
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kStalker),
                .layer = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(
            stalkerEntity_,
            component::Parallax{
                .factor = glm::vec2(stalkerPoints_[currentStalkerPointIndex_].parallaxFactor),
                .basePosition = stalkerPoints_[currentStalkerPointIndex_].position,
            }
        );
        registry_.addComponent(
            stalkerEntity_,
            component::MenuStalker{
                .hidePosition = stalkerPoints_[currentStalkerPointIndex_].position,
                .peekOffset = stalkerPoints_[currentStalkerPointIndex_].peekOffset,
                .minDistance = 0.1f,
                .maxDistance = 0.15f,
                .peekSpeed = 4.f,
            }
        );
        registry_.addComponent(stalkerEntity_, component::FovMasked{});
      }

      {  // Cursor FOV
        fovEntity_ = registry_.createEntity();
        registry_.addComponent(fovEntity_, component::EntityName{ .name = "cursor_fov" });
        registry_.addComponent(fovEntity_, component::Transform{ .position = glm::vec2(0.f) });
        registry_.addComponent(
            fovEntity_,
            component::FieldOfView{
                .innerRadius = 0.35f,
                .outerRadius = 0.55f,
                .fovAngle = 360.f,
                .smoothnessAngle = 0.f,
                .smoothnessDistance = 0.55f,
                .darkness = 0.4f,
            }
        );
      }

      {  // Title
        auto title{ registry_.createEntity() };
        registry_.addComponent(title, component::EntityName{ .name = "title" });
        registry_.addComponent(
            title,
            component::Transform{
                .scale = glm::vec2{ 2.f, 0.4f },
                .position = glm::vec2{ 0.f, 0.6f },
                .rotation = 0.f,
            }
        );
        registry_.addComponent(
            title,
            component::Sprite{
                .color = glm::vec4{ 1.f },
                .uvScale = glm::vec2{ 1.f },
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kMainMenuTitle),
                .angleOffset = 0.f,
                .layer = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(title, component::FovMasked{});
        registry_.addComponent(
            title,
            component::Parallax{
                .factor = glm::vec2{ 0.f, 0.1f },
                .basePosition = glm::vec2{ 0.f, 0.6f },
            }
        );
      }

      {  // Grass
        std::vector<glm::vec2> positions{
          glm::vec2{ -3.f, -0.8f },  glm::vec2{ -2.f, -0.4f }, glm::vec2{ -1.f, 0.f },   glm::vec2{ 3.f, -0.5f },
          glm::vec2{ 2.f, -0.2f },   glm::vec2{ 1.f, 0.f },    glm::vec2{ -5.f, -0.3f }, glm::vec2{ -2.f, -0.8f },
          glm::vec2{ -1.5f, -0.1f }, glm::vec2{ 2.2f, -0.7f }, glm::vec2{ 1.6f, -0.6f }, glm::vec2{ 0.2f, -0.5f },
        };

        int index{ 0 };
        for (const auto& position : positions) {
          auto grass{ registry_.createEntity() };
          registry_.addComponent(grass, component::EntityName{ .name = std::format("grass_{}", index++) });
          registry_.addComponent(
              grass,
              component::Transform{
                  .scale = glm::vec2(0.3f),
                  .position = position,
              }
          );
          registry_.addComponent(
              grass,
              component::Sprite{
                  .color = glm::vec4(0.1f, 0.5f, 0.1f, 1.f),
                  .uvScale = glm::vec2(1.f),
                  .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kGrass1),
                  .layer = renderer::Layer::Foreground,
              }
          );
          registry_.addComponent(
              grass,
              component::Parallax{
                  .factor = glm::vec2(0.1f),
                  .basePosition = position,
              }
          );
        }
      }

      {  // Trunks
        std::vector<glm::vec2> positions{
          glm::vec2{ -2.f, 1.f }, glm::vec2{ -1.f, 1.1f },  glm::vec2{ 0.5f, 0.9f },
          glm::vec2{ 1.2f, 1.f }, glm::vec2{ -0.2f, 1.2f },
        };

        int index{ 0 };
        for (const auto& position : positions) {
          auto trunk{ registry_.createEntity() };
          registry_.addComponent(trunk, component::EntityName{ .name = std::format("trunt_{}", index++) });
          registry_.addComponent(
              trunk,
              component::Transform{
                  .scale = glm::vec2(0.3f, backgroundSize_.y),
                  .position = position,
              }
          );
          registry_.addComponent(
              trunk,
              component::Sprite{
                  .color = glm::vec4(1.f),
                  .uvScale = glm::vec2(1.f),
                  .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kTrunk),
                  .layer = renderer::Layer::Foreground,
              }
          );
          registry_.addComponent(
              trunk,
              component::Parallax{
                  .factor = glm::vec2(0.15f),
                  .basePosition = position,
              }
          );
        }

        positions = {
          glm::vec2{ -1.5f, 2.35f },
          glm::vec2{ 1.f, 2.f },
          glm::vec2{ 0.f, 2.2f },
          glm::vec2{ -0.3f, 2.3f },
        };
        for (const auto& position : positions) {
          auto trunk{ registry_.createEntity() };
          registry_.addComponent(trunk, component::EntityName{ .name = std::format("trunt_{}", index++) });
          registry_.addComponent(
              trunk,
              component::Transform{
                  .scale = glm::vec2(0.2f, backgroundSize_.y),
                  .position = position,
              }
          );
          registry_.addComponent(
              trunk,
              component::Sprite{
                  .color = glm::vec4(1.f),
                  .uvScale = glm::vec2(1.f),
                  .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kTrunk),
                  .layer = renderer::Layer::Ground,
              }
          );
          registry_.addComponent(
              trunk,
              component::Parallax{
                  .factor = glm::vec2(0.05f),
                  .basePosition = position,
              }
          );
        }
      }
    }
  }

}  // namespace ls
