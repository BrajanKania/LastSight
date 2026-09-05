#include "game/scenes/world_scene.hpp"

#include <SDL3/SDL_log.h>
#include <glad/gl.h>
#include <imgui.h>

#include <cassert>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/components/camera.hpp"
#include "engine/components/collider.hpp"
#include "engine/components/entity_name.hpp"
#include "engine/components/interactable.hpp"
#include "engine/components/particle_emitter.hpp"
#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/core/update_context.hpp"
#include "engine/ecs/registry.hpp"
#include "engine/input/input_context.hpp"
#include "engine/input/types.hpp"
#include "engine/interactions/interaction_system.hpp"
#include "engine/particles/particle_system.hpp"
#include "engine/physics/physics_system.hpp"
#include "engine/renderer/layer.hpp"
#include "engine/renderer/material/material_names.hpp"
#include "engine/renderer/render_phase.hpp"
#include "engine/renderer/render_pipeline.hpp"
#include "engine/serialization/scene_serializer.hpp"
#include "game/actions/aim.hpp"
#include "game/actions/interact.hpp"
#include "game/actions/move.hpp"
#include "game/actions/select_slot_0.hpp"
#include "game/actions/select_slot_1.hpp"
#include "game/actions/select_slot_2.hpp"
#include "game/actions/select_slot_3.hpp"
#include "game/actions/select_slot_4.hpp"
#include "game/actions/select_slot_5.hpp"
#include "game/actions/shoot.hpp"
#include "game/actions/sprint.hpp"
#include "game/components/camera_shake.hpp"
#include "game/components/enemy.hpp"
#include "game/components/field_of_view.hpp"
#include "game/components/fov_masked.hpp"
#include "game/components/health.hpp"
#include "game/components/inventory.hpp"
#include "game/components/item_pickup.hpp"
#include "game/components/lamp.hpp"
#include "game/components/movement_settings.hpp"
#include "game/components/player.hpp"
#include "game/components/player_camera_settings.hpp"
#include "game/components/player_fov_settings.hpp"
#include "game/components/player_state.hpp"
#include "game/components/post_process_settings.hpp"
#include "game/components/stamina.hpp"
#include "game/components/weapon.hpp"
#include "game/extraction/extraction_system.hpp"
#include "game/items/weapon_config.hpp"
#include "game/particles/fire.hpp"
#include "game/renderer/passes/fov_pass.hpp"
#include "game/renderer/passes/lit_pass.hpp"
#include "game/renderer/passes/post_process_pass.hpp"
#include "game/scenes/texture_names.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/combat_system.hpp"
#include "game/systems/equip_system.hpp"
#include "game/systems/fov_system.hpp"
#include "game/systems/inventory_system.hpp"
#include "game/systems/lamp_system.hpp"
#include "game/systems/player_system.hpp"
#include "game/systems/post_process_system.hpp"
#include "game/systems/projectile_system.hpp"
#include "game/ui/inventory_panel.hpp"
#include "game/ui/panel_names.hpp"

namespace ls {

  void WorldScene::onEnter() {
    assert(engineCtx_.eventQueue != nullptr && "[WorldScene] requires a valid EventQueue!");
    assert(engineCtx_.textureManager != nullptr && "[WorldScene] requires a valid TextureManager!");
    assert(engineCtx_.materialManager != nullptr && "[WorldScene] requires a valid MaterailManager!");

    serialization::SceneSerializer serializer(getSceneContext(), engineCtx_);
    serializer.loadScene(asset_system::scene(scene::kWorld));
    for (auto entity : registry_.view<component::Player>()) {
      player_ = entity;
      break;
    }

    // genereteEntities();

    // for (auto entity : registry_.view<component::Sprite>()) {
    //   auto& sprite{ registry_.getComponent<component::Sprite>(entity) };
    //   sprite.materialHandle = engineCtx_.materialManager->getHandle(material_name::kLit);
    // }
    //
    // for (auto entity : registry_.view<component::EquippedSprite>()) {
    //   auto& equippedSprite{ registry_.getComponent<component::EquippedSprite>(entity) };
    //   equippedSprite.materialHandle = engineCtx_.materialManager->getHandle(material_name::kLit);
    // }

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
              .iconTextureHandle = engineCtx_.textureManager->getHandle(texture_name::kWorldPistol),
              .worldTextureHandle = engineCtx_.textureManager->getHandle(texture_name::kWorldPistol),
              .equippedTextureHandle = engineCtx_.textureManager->getHandle(texture_name::kEquippedPistol),
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
              .iconTextureHandle = engineCtx_.textureManager->getHandle(texture_name::kWorldRifle),
              .worldTextureHandle = engineCtx_.textureManager->getHandle(texture_name::kWorldRifle),
              .equippedTextureHandle = engineCtx_.textureManager->getHandle(texture_name::kEquippedRifle),
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
      .textureManager = *engineCtx_.textureManager,
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

  void WorldScene::genereteEntities() {
    {  // world entities

      {  // Background
        auto background{ registry_.createEntity() };
        registry_.addComponent(background, component::EntityName{ .name = "background" });
        registry_.addComponent(background, component::Transform{ .scale = glm::vec2(5.f) });
        registry_.addComponent(
            background,
            component::Sprite{
                .uvScale = glm::vec2(2.5f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kGrass),
                .layer = renderer::Layer::Background,
            }
        );
      }

      {  // Player
        player_ = registry_.createEntity();
        registry_.addComponent(player_, component::Player{});
        registry_.addComponent(
            player_,
            component::EntityName{
                .name = "Player",
            }
        );
        registry_.addComponent(
            player_,
            component::Transform{
                .scale = glm::vec2(0.5f),
            }
        );
        registry_.addComponent(
            player_,
            component::Sprite{
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kPlayer),
                .angleOffset = -90.f,
                .layer = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(player_, component::Velocity{});
        registry_.addComponent(
            player_,
            component::MovementSettings{
                .sprintSpeed = 2.f,
            }
        );
        registry_.addComponent(
            player_,
            component::FieldOfView{
                .innerRadius = 1.0f,
                .outerRadius = 6.f,
                .smoothnessDistance = 2.f,
            }
        );
        registry_.addComponent(
            player_,
            component::PlayerFovSettings{
                .baseAngle = 60.f,
                .aimAngle = 10.f,
                .lowStaminaAngle = 5.f,
                .baseSmoothnessAngle = 40.f,
                .aimSmoothnessAngle = 10.f,
                .lowStaminaSmoothnessAngle = 20.f,
                .baseDarkness = 0.6f,
                .aimDarkness = 0.55f,
                .lowStaminaThreshold = 0.4,
                .transitionSpeed = 4.f,
            }
        );
        registry_.addComponent(
            player_,
            component::Collider{
                .type = component::ColliderType::Circle,
                .radius = 0.5f,
            }
        );
        registry_.addComponent(player_, component::Inventory{ 6 });
        registry_.addComponent(
            player_,
            component::Health{
                .max = 100.f,
                .current = 100.f,
            }
        );
        registry_.addComponent(
            player_,
            component::Stamina{
                .max = 100.f,
                .current = 100.f,
                .regenRate = 10.f,
                .sprintCostRate = 20.f,
                .aimCostRate = 10.f,
                .recoveryThresholdRatio = 0.2f,
            }
        );
        registry_.addComponent(
            player_,
            component::PostProcessSettings{
                .damageVignetteColor = glm::vec3(0.8f, 0.0f, 0.0f),
                .damageInnerRadius = 0.1f,
                .damageOuterRadius = 0.7f,
                .maxDamageDesaturation = 0.5f,
                .staminaInnerRadius = 0.1f,
                .staminaOuterRadius = 0.3f,
                .maxStaminaDesaturation = 0.8f,
            }
        );
        registry_.addComponent(player_, component::PlayerState{});
        registry_.addComponent(
            player_,
            component::PlayerCameraSettings{
                .baseZoom = 1.f,
                .aimZoom = 1.2f,
                .sprintZoom = 0.95f,
                .zoomSpeed = 0.5f,
                .baseFollowSpeed = 5.f,
                .aimFollowSpeed = 1.f,
                .aimOffsetWeight = 0.3f,
                .maxAimOffset = 0.5f,
            }
        );
      }

      {  // Camera
        auto camera{ registry_.createEntity() };
        registry_.addComponent(
            camera,
            component::Camera{
                .orthographicSize = 6.f,
                .zoom = 1.f,
            }
        );
        registry_.addComponent(
            camera,
            component::EntityName{
                .name = "Camera",
            }
        );
        registry_.addComponent(camera, component::Transform{});
        registry_.addComponent(
            camera,
            component::CameraShake{
                .stiffness = 180.f,
                .damping = 20.f,
                .maxSpringOffset = 0.4f,
                .traumaDecay = 1.f,
                .maxTraumaOffset = 0.35f,
                .traumaFrequency = 8.f,
            }
        );
      }

      {  // Container
        auto container{ registry_.createEntity() };
        registry_.addComponent(container, component::EntityName{ .name = "container" });
        registry_.addComponent(
            container,
            component::Transform{
                .scale = glm::vec2(0.5f),
                .position = glm::vec2(1.f),
            }
        );
        registry_.addComponent(
            container,
            component::Sprite{
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kContainer),
                .layer = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(
            container,
            component::Collider{
                .type = component::ColliderType::AABB,
                .halfExtents = glm::vec2(0.5f),
            }
        );
        registry_.addComponent(
            container,
            component::ParticleEmitter{ particle::preset::fire(
                engineCtx_.materialManager->getHandle(material_name::kLit),
                engineCtx_.textureManager->getHandle(texture_name::kWhite)
            ) }
        );
        registry_.addComponent(container, component::Lamp{});
        registry_.addComponent(container, component::FovMasked{});
        registry_.addComponent(
            container,
            component::Interactable{
                .radius = 0.8f,
            }
        );
      }

      {  // Enemy
        auto enemy{ registry_.createEntity() };
        registry_.addComponent(enemy, component::EntityName{ .name = "enemy" });
        registry_.addComponent(enemy, component::Enemy{});
        registry_.addComponent(
            enemy,
            component::Transform{
                .scale = glm::vec2(0.8f),
                .position = glm::vec2(-1.f),
            }
        );
        registry_.addComponent(
            enemy,
            component::Sprite{
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kEnemy),
                .layer = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(
            enemy,
            component::Collider{
                .type = component::ColliderType::Circle,
                .radius = 0.3f,
            }
        );
        registry_.addComponent(enemy, component::FovMasked{});
      }

      {  // Container 2
        auto container2{ registry_.createEntity() };
        registry_.addComponent(container2, component::EntityName{ .name = "container_2" });
        registry_.addComponent(
            container2,
            component::Transform{
                .scale = glm::vec2(0.8f),
                .position = glm::vec2(1.f, -1.f),
                .rotation = 30.f,
            }
        );
        registry_.addComponent(
            container2,
            component::Sprite{
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kContainer),
                .layer = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(
            container2,
            component::Collider{
                .type = component::ColliderType::OBB,
                .halfExtents = glm::vec2(0.5f),
            }
        );
      }

      {  // Pistol
        auto pistol{ registry_.createEntity() };
        registry_.addComponent(pistol, component::EntityName{ .name = "pistol" });
        registry_.addComponent(
            pistol,
            component::Transform{
                .scale = glm::vec2(0.4f),
                .position = glm::vec2(-1.f, 0.f),
                .rotation = 50.f,
            }
        );
        registry_.addComponent(
            pistol,
            component::Sprite{
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kWorldPistol),
                .angleOffset = -90.f,
                .layer = renderer::Layer::Ground,
            }
        );
        registry_.addComponent(
                  pistol,
                component::ItemPickup{
                  .itemStack = {
                    .itemId = "weapon_pistol",
                    .count = 1,
                  },
                }
              );
        registry_.addComponent(
            pistol,
            component::Interactable{
                .radius = 0.5f,
            }
        );
      }

      {  // Rifle
        auto rifle{ registry_.createEntity() };
        registry_.addComponent(rifle, component::EntityName{ .name = "rifle" });
        registry_.addComponent(
            rifle,
            component::Transform{
                .scale = glm::vec2(0.6f),
                .position = glm::vec2(-1.f, 1.f),
                .rotation = 0.f,
            }
        );
        registry_.addComponent(
            rifle,
            component::Sprite{
                .uvScale = glm::vec2(1.f),
                .textureHandle = engineCtx_.textureManager->getHandle(texture_name::kWorldRifle),
                .angleOffset = -90.f,
                .layer = renderer::Layer::Ground,
            }
        );
        registry_.addComponent(
                  rifle,
                component::ItemPickup{
                  .itemStack = {
                    .itemId = "weapon_rifle",
                    .count = 1,
                  },
                }
              );
        registry_.addComponent(
            rifle,
            component::Interactable{
                .radius = 0.5f,
            }
        );
      }
    }
  }

}  // namespace ls
