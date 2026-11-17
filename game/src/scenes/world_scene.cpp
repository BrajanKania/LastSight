#include "scenes/world_scene.hpp"

#include <SDL3/SDL_log.h>
#include <glad/gl.h>
#include <imgui.h>

#include <cassert>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "actions/aim.hpp"
#include "actions/interact.hpp"
#include "actions/move.hpp"
#include "actions/select_slot_0.hpp"
#include "actions/select_slot_1.hpp"
#include "actions/select_slot_2.hpp"
#include "actions/select_slot_3.hpp"
#include "actions/select_slot_4.hpp"
#include "actions/select_slot_5.hpp"
#include "actions/shoot.hpp"
#include "actions/sprint.hpp"
#include "components/field_of_view.hpp"
#include "components/player.hpp"
#include "components/post_process_settings.hpp"
#include "components/weapon.hpp"
#include "extraction/extraction_system.hpp"
#include "items/weapon_config.hpp"
#include "ls/asset/asset_system.hpp"
#include "ls/asset/asset_types.hpp"
#include "ls/components/camera.hpp"
#include "ls/components/transform.hpp"
#include "ls/core/update_context.hpp"
#include "ls/ecs/registry.hpp"
#include "ls/input/input_context.hpp"
#include "ls/input/types.hpp"
#include "ls/interactions/interaction_system.hpp"
#include "ls/particles/particle_system.hpp"
#include "ls/physics/physics_system.hpp"
#include "ls/renderer/material/material_names.hpp"
#include "ls/renderer/render_phase.hpp"
#include "ls/renderer/render_pipeline.hpp"
#include "ls/serialization/scene_serializer.hpp"
#include "renderer/passes/fov_pass.hpp"
#include "renderer/passes/lit_pass.hpp"
#include "renderer/passes/post_process_pass.hpp"
#include "scenes/texture_names.hpp"
#include "systems/camera_system.hpp"
#include "systems/combat_system.hpp"
#include "systems/equip_system.hpp"
#include "systems/fov_system.hpp"
#include "systems/inventory_system.hpp"
#include "systems/lamp_system.hpp"
#include "systems/player_system.hpp"
#include "systems/post_process_system.hpp"
#include "systems/projectile_system.hpp"
#include "ui/inventory_panel.hpp"
#include "ui/panel_names.hpp"

namespace ls {

  void WorldScene::onEnter() {
    assert(engineCtx_.eventQueue != nullptr && "[WorldScene] requires a valid EventQueue!");
    assert(engineCtx_.materialManager != nullptr && "[WorldScene] requires a valid MaterailManager!");

    serialization::SceneSerializer serializer(getSceneContext(), engineCtx_);
    serializer.loadScene(asset_system::scene(scene::kWorld));
    for (auto entity : registry_.view<component::Player>()) {
      player_ = entity;
      break;
    }

    engineCtx_.renderPipeline->addPass<renderer::LitPass>(renderer::RenderPhase::MainPass);
    engineCtx_.renderPipeline->addPass<renderer::FovPass>(renderer::RenderPhase::MainPass);
    engineCtx_.renderPipeline->addPass<renderer::PostProcessPass>(renderer::RenderPhase::MainPass);

    uiManager_.addPanel<ui::InventoryPanel>(ui::panel::kInventory, itemRegistry_);

    inputManager_.bindAxis2D<action::Move>(input::Key::W, input::Key::S, input::Key::A, input::Key::D);
    inputManager_.bindButton<action::Shoot>(input::Button::Left);
    inputManager_.bindKey<action::Sprint>(input::Key::LShift);
    inputManager_.bindKey<action::Interact>(input::Key::E);
    inputManager_.bindKey<action::SelectSlot0>(input::Key::Num1);
    inputManager_.bindKey<action::SelectSlot1>(input::Key::Num2);
    inputManager_.bindKey<action::SelectSlot2>(input::Key::Num3);
    inputManager_.bindKey<action::SelectSlot3>(input::Key::Num4);
    inputManager_.bindKey<action::SelectSlot4>(input::Key::Num5);
    inputManager_.bindKey<action::SelectSlot5>(input::Key::Num6);
    inputManager_.bindButton<action::Aim>(input::Button::Right);

    {  // Items

      itemRegistry_.registerItem(
          item::ItemDefinition{
              .id = "weapon_pistol",
              .materialHandle = engineCtx_.materialManager->getHandle(material_name::kLit),
              .iconTextureHandle = engineCtx_.assetManager->findAssetHandleByPath(texture_name::kWorldPistol)
                                       .as<asset::AssetType::Texture2D>(),
              .equippedTextureHandle = engineCtx_.assetManager->findAssetHandleByPath(texture_name::kEquippedPistol)
                                           .as<asset::AssetType::Texture2D>(),
              .worldTextureHandle = engineCtx_.assetManager->findAssetHandleByPath(texture_name::kWorldPistol)
                                        .as<asset::AssetType::Texture2D>(),
              .equippedScale = glm::vec2(0.5f),
              .equippedOffset = glm::vec2(0.f, 0.2f),
              .equippedAngleOffset = -90.f,
              .canStack = false,
              .maxStackSize = 1,
              .weaponConfig =
                  item::WeaponConfig{
                      .isAutomatic = false,
                      .fireRate = 0.1f,
                      .initialSpeed = 6.f,
                      .bulletScale = glm::vec2(0.025f),
                      .bulletLifetime = 3.f,
                      .transitionSpeed = 8.f,
                      .muzzleOffset = glm::vec2(0.f, 0.14f),
                      .hipOffset = glm::vec2(0.12f, 0.21f),
                      .aimOffset = glm::vec2(0.f, 0.27f),
                      .hipRecoil =
                          component::Weapon::RecoilConfig{
                              .baseSpread = 10.f,
                              .weaponImpulse = 5.f,
                              .weaponAngularImpulse = 150.f,
                              .cameraImpulse = 1.f,
                              .cameraTrauma = 0.1f,
                              .maxSpringOffset = 0.15f,
                              .springStiffness = 400.f,
                              .damping = 30.f,
                              .maxSpringRotation = 90.f,
                              .angularSpringStiffness = 80.f,
                              .angularDamping = 8.f,
                          },
                      .aimRecoil =
                          component::Weapon::RecoilConfig{
                              .baseSpread = 5.f,
                              .weaponImpulse = 8.f,
                              .weaponAngularImpulse = 100.f,
                              .cameraImpulse = 1.2f,
                              .cameraTrauma = 0.1f,
                              .maxSpringOffset = 0.15f,
                              .springStiffness = 400.f,
                              .damping = 40.f,
                              .maxSpringRotation = 30.f,
                              .angularSpringStiffness = 100.f,
                              .angularDamping = 8.f,
                          },
                  },
          }
      );

      itemRegistry_.registerItem(
          item::ItemDefinition{
              .id = "weapon_rifle",
              .materialHandle = engineCtx_.materialManager->getHandle(material_name::kLit),
              .iconTextureHandle = engineCtx_.assetManager->findAssetHandleByPath(texture_name::kWorldRifle)
                                       .as<asset::AssetType::Texture2D>(),
              .equippedTextureHandle = engineCtx_.assetManager->findAssetHandleByPath(texture_name::kEquippedRifle)
                                           .as<asset::AssetType::Texture2D>(),
              .worldTextureHandle = engineCtx_.assetManager->findAssetHandleByPath(texture_name::kWorldRifle)
                                        .as<asset::AssetType::Texture2D>(),
              .equippedScale = glm::vec2(1.2f),
              .equippedOffset = glm::vec2(0.08f, 0.3f),
              .equippedAngleOffset = -90.f,
              .canStack = false,
              .maxStackSize = 1,
              .weaponConfig =
                  item::WeaponConfig{
                      .isAutomatic = true,
                      .fireRate = 0.12f,
                      .initialSpeed = 10.f,
                      .bulletScale = glm::vec2(0.025f),
                      .bulletLifetime = 3.f,
                      .transitionSpeed = 4.f,
                      .muzzleOffset = glm::vec2(0.f, 0.3f),
                      .hipOffset = glm::vec2(0.12f, 0.27f),
                      .aimOffset = glm::vec2(0.f, 0.38f),
                      .hipRecoil =
                          component::Weapon::RecoilConfig{
                              .baseSpread = 10.f,
                              .weaponImpulse = 12.f,
                              .weaponAngularImpulse = 150.f,
                              .cameraImpulse = 1.2f,
                              .cameraTrauma = 0.18f,
                              .maxSpringOffset = 0.15f,
                              .springStiffness = 200.f,
                              .damping = 30.f,
                              .maxSpringRotation = 120.f,
                              .angularSpringStiffness = 150.f,
                              .angularDamping = 10.f,
                          },
                      .aimRecoil =
                          component::Weapon::RecoilConfig{
                              .baseSpread = 3.f,
                              .weaponImpulse = 12.f,
                              .weaponAngularImpulse = 80.f,
                              .cameraImpulse = 1.5f,
                              .cameraTrauma = 0.13f,
                              .maxSpringOffset = 0.15f,
                              .springStiffness = 400.f,
                              .damping = 40.f,
                              .maxSpringRotation = 30.f,
                              .angularSpringStiffness = 150.f,
                              .angularDamping = 10.f,
                          },
                  },
          }
      );
    }  // namespace ls
  }

  void WorldScene::onExit() {}

  void WorldScene::onResize(int width, int height) {}

  void WorldScene::handleInput(input::InputContext& inputCtx) { inputManager_.update(inputCtx); }

  void WorldScene::update(float dt) {
    processEvents();

    UpdateContext ctx{
      .registry = registry_,
      .eventQueue = eventQueue_,
      .materialManager = *engineCtx_.materialManager,
      .assetManager = *engineCtx_.assetManager,
      .inputManager = inputManager_,
      .dt = dt,
    };

    player_system::update(ctx);
    interaction_system::update(ctx);

    combat_system::update(ctx);
    inventory_system::update(ctx, itemRegistry_);
    equip_system::update(ctx, itemRegistry_);

    physics_system::update(ctx);

    projectile_system::update(ctx);
    lamp_system::update(ctx);

    particle_system::update(ctx);

    camera_system::follow(ctx, player_);
    camera_system::update(ctx);

    fov_system::update(ctx);
    post_process_system::update(ctx);

    registry_.purgeDestroyedEntities();
    eventQueue_.clear();
  }

  void WorldScene::render() {
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

    for (auto entity : registry_.view<component::PostProcessSettings>()) {
      const auto& settings{ registry_.getComponent<component::PostProcessSettings>(entity) };

      if (auto* material{ engineCtx_.materialManager->get(material_name::kPostProcess) }) {
        material->setProperty("uDamageVignetteColor", settings.damageVignetteColor);
        material->setProperty("uDamageInnerRadius", settings.damageInnerRadius);
        material->setProperty("uDamageOuterRadius", settings.damageOuterRadius);
        material->setProperty("uMaxDamageDesaturation", settings.maxDamageDesaturation);
        material->setProperty("uDamageIntensity", settings.currentDamageIntensity);

        material->setProperty("uStaminaInnerRadius", settings.staminaInnerRadius);
        material->setProperty("uStaminaOuterRadius", settings.staminaOuterRadius);
        material->setProperty("uMaxStaminaDesaturation", settings.maxStaminaDesaturation);
        material->setProperty("uStaminaIntensity", settings.currentStaminaIntensity);
      }
    }

    extraction_system::extractGameRenderCommands(engineCtx_.renderPipeline->getCommandBuffer(), registry_);
  }

  void WorldScene::renderUI() { uiManager_.render(getUIContext()); }

  void WorldScene::processEvents() {}

}  // namespace ls
