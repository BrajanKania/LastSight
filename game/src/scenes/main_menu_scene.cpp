#include "scenes/main_menu_scene.hpp"

#include <imgui.h>

#include <cassert>
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <vector>

#include "components/field_of_view.hpp"
#include "components/menu_stalker.hpp"
#include "components/menu_stalker_point.hpp"
#include "extraction/extraction_system.hpp"
#include "ls/asset/asset_system.hpp"
#include "ls/components/camera.hpp"
#include "ls/components/entity_name.hpp"
#include "ls/components/parallax.hpp"
#include "ls/components/transform.hpp"
#include "ls/core/random_system.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/input/input_context.hpp"
#include "ls/input/input_manager.hpp"
#include "ls/input/input_system.hpp"
#include "ls/renderer/material/material_manager.hpp"
#include "ls/renderer/material/material_names.hpp"
#include "ls/renderer/render_phase.hpp"
#include "ls/renderer/render_pipeline.hpp"
#include "ls/serialization/scene_serializer.hpp"
#include "ls/systems/parallax_system.hpp"
#include "renderer/passes/fov_pass.hpp"
#include "renderer/passes/lit_pass.hpp"
#include "scenes/scene_names.hpp"
#include "systems/camera_system.hpp"
#include "ui/main_menu_panel.hpp"
#include "ui/panel_names.hpp"

namespace ls {

  void MainMenuScene::onEnter() {
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
      .assetManager = *engineCtx_.assetManager,
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

}  // namespace ls
