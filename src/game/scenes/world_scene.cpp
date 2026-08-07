#include "game/scenes/world_scene.hpp"

#include <SDL3/SDL_log.h>
#include <glad/gl.h>
#include <imgui.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "engine/components/collider.hpp"
#include "engine/components/entity_name.hpp"
#include "engine/components/interactable.hpp"
#include "engine/components/particle_emitter.hpp"
#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/core/update_context.hpp"
#include "engine/events/set_panel_visibility.hpp"
#include "engine/events/toggle_panel.hpp"
#include "engine/gfx/framebuffer.hpp"
#include "engine/input/types.hpp"
#include "engine/interactions/interaction_system.hpp"
#include "engine/particles/particle_system.hpp"
#include "engine/physics/physics_system.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/layer.hpp"
#include "engine/renderer/passes/compose_pass.hpp"
#include "engine/renderer/passes/fov_pass.hpp"
#include "engine/renderer/passes/lit_pass.hpp"
#include "engine/renderer/passes/post_process_pass.hpp"
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
#include "game/actions/toggle_debug.hpp"
#include "game/components/camera.hpp"
#include "game/components/camera_shake.hpp"
#include "game/components/enemy.hpp"
#include "game/components/field_of_view.hpp"
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
#include "game/particles/fire.hpp"
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
#include "game/ui/debug_toolbox_panel.hpp"
#include "game/ui/entity_explorer_panel.hpp"
#include "game/ui/inventory_panel.hpp"
#include "game/ui/panel_names.hpp"
#include "game/ui/render_pipeline_debug_panel.hpp"

namespace ls {

  void WorldScene::onEnter() {
    textureManager_.load(texture_name::kWhite, asset_system::texture(texture_name::kWhite));
    textureManager_.load(texture_name::kGrass, asset_system::texture(texture_name::kGrass));
    textureManager_.load(texture_name::kContainer, asset_system::texture(texture_name::kContainer));
    textureManager_.load(texture_name::kPlayer, asset_system::texture(texture_name::kPlayer));
    textureManager_.load(texture_name::kWorldPistol, asset_system::texture(texture_name::kWorldPistol));
    textureManager_.load(texture_name::kEquippedPistol, asset_system::texture(texture_name::kEquippedPistol));
    textureManager_.load(texture_name::kBullet, asset_system::texture(texture_name::kBullet));
    textureManager_.load(texture_name::kWorldRifle, asset_system::texture(texture_name::kWorldRifle));
    textureManager_.load(texture_name::kEquippedRifle, asset_system::texture(texture_name::kEquippedRifle));
    textureManager_.load(texture_name::kEnemy, asset_system::texture(texture_name::kEnemy));

    {  // world entities

      {  // Background
        auto background{ registry_.createEntity() };
        registry_.addComponent(background, component::Transform{ .scale = glm::vec2(5.f) });
        registry_.addComponent(
            background,
            component::Sprite{
                .uvScale = glm::vec2(2.5f),
                .textureId = textureManager_.getId(texture_name::kGrass),
                .zIndex = renderer::Layer::Background,
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
                .textureId = textureManager_.getId(texture_name::kPlayer),
                .angleOffset = -90.f,
                .zIndex = renderer::Layer::Entities,
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
        registry_.addComponent(
            player_,
            component::Inventory{
                .maxSlots = 6,
            }
        );
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
                .orthographicSize = 5.f,
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
                .maxTraumaOffset = 0.3f,
                .traumaFrequency = 8.f,
            }
        );
      }

      {  // Container
        auto container{ registry_.createEntity() };
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
                .textureId = textureManager_.getId(texture_name::kContainer),
                .zIndex = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(
            container,
            component::Collider{
                .type = component::ColliderType::AABB,
                .halfExtents = glm::vec2(0.5f),
            }
        );
        registry_.addComponent(container, component::ParticleEmitter{ particle::preset::fire() });
        registry_.addComponent(container, component::Lamp{});
        registry_.addComponent(
            container,
            component::Interactable{
                .radius = 0.8f,
            }
        );
      }

      {  // Enemy
        auto enemy{ registry_.createEntity() };
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
                .textureId = textureManager_.getId(texture_name::kEnemy),
                .zIndex = renderer::Layer::Entities,
            }
        );
        registry_.addComponent(
            enemy,
            component::Collider{
                .type = component::ColliderType::Circle,
                .radius = 0.3f,
            }
        );
      }

      {  // Container 2
        auto container2{ registry_.createEntity() };
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
                .textureId = textureManager_.getId(texture_name::kContainer),
                .zIndex = renderer::Layer::Entities,
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
                .textureId = textureManager_.getId(texture_name::kWorldPistol),
                .angleOffset = -90.f,
                .zIndex = renderer::Layer::Ground,
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
                .textureId = textureManager_.getId(texture_name::kWorldRifle),
                .angleOffset = -90.f,
                .zIndex = renderer::Layer::Ground,
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

    worldFBO_ = std::make_shared<gfx::Framebuffer>();
    fovFBO_ = std::make_shared<gfx::Framebuffer>();
    processedFBO_ = std::make_shared<gfx::Framebuffer>();

    renderPipeline_.addPass<renderer::LitPass>(worldFBO_);
    renderPipeline_.addPass<renderer::FovPass>(fovFBO_, worldFBO_);
    renderPipeline_.addPass<renderer::PostProcessPass>(processedFBO_, fovFBO_);
    renderPipeline_.addPass<renderer::ComposePass>(processedFBO_);

    uiManager_.addPanel<ui::DebugToolboxPanel>(ui::panel::kDebugToolbox);
    uiManager_.addPanel<ui::RenderPipelineDebugPanel>(ui::panel::kRenderPipelineDebug, renderPipeline_);
    uiManager_.addPanel<ui::EntityExplorerPanel>(ui::panel::kEntityExplorer);
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

    debugInputManager_.bindKey<action::ToggleDebug>(input::Key::Grave);

    {  // Items

      itemRegistry_.registerItem(
          item::ItemDefinition{
              .id = "weapon_pistol",
              .iconTextureId = textureManager_.getId(texture_name::kWorldPistol),
              .worldTextureId = textureManager_.getId(texture_name::kWorldPistol),
              .equippedTextureId = textureManager_.getId(texture_name::kEquippedPistol),
              .angleOffset = -90.f,
              .equippedScale = glm::vec2(0.5f),
              .equippedOffset = glm::vec2(0.f, 0.2f),
              .canStack = false,
              .maxStackSize = 1,
              .weaponConfig =
                  item::WeaponConfig{
                      .isAutomatic = false,
                      .fireRate = 0.f,
                      .initialSpeed = 4.f,
                      .bulletScale = glm::vec2(0.03f),
                      .bulletLifetime = 3.f,
                      .barrelOffset = glm::vec2(0.f, 0.32f),
                      .recoilImpulse = 1.f,
                      .recoilTrauma = 0.1f,
                  },
          }
      );

      itemRegistry_.registerItem(
          item::ItemDefinition{
              .id = "weapon_rifle",
              .iconTextureId = textureManager_.getId(texture_name::kWorldRifle),
              .worldTextureId = textureManager_.getId(texture_name::kWorldRifle),
              .equippedTextureId = textureManager_.getId(texture_name::kEquippedRifle),
              .angleOffset = -90.f,
              .equippedScale = glm::vec2(1.2f),
              .equippedOffset = glm::vec2(0.08f, 0.3f),
              .canStack = false,
              .maxStackSize = 1,
              .weaponConfig =
                  item::WeaponConfig{
                      .isAutomatic = true,
                      .fireRate = 0.1f,
                      .initialSpeed = 6.f,
                      .bulletScale = glm::vec2(0.03f),
                      .bulletLifetime = 3.f,
                      .barrelOffset = glm::vec2(-0.082f, 0.6f),
                      .recoilImpulse = 2.f,
                      .recoilTrauma = 0.15f,
                  },
          }
      );
    }  // namespace ls
  }

  void WorldScene::onExit() {}

  void WorldScene::onResize(int width, int height) {
    worldFBO_->resize(width, height);
    fovFBO_->resize(width, height);
    processedFBO_->resize(width, height);
  }

  void WorldScene::handleInput() {
    debugInputManager_.update(false, false);

    if (debugInputManager_.getActionState<action::ToggleDebug>() == input::ActionState::JustPressed) {
      eventQueue_.publish<event::TogglePanel>(event::TogglePanel{
          .name = ui::panel::kDebugToolbox,
      });

      eventQueue_.publish(
          event::SetPanelVisibility{
              .name = ui::panel::kEntityExplorer,
              .visible = false,
          }
      );

      eventQueue_.publish(
          event::SetPanelVisibility{
              .name = ui::panel::kRenderPipelineDebug,
              .visible = false,
          }
      );
    }

    ImGuiIO& io{ ImGui::GetIO() };
    inputManager_.update(io.WantCaptureKeyboard, io.WantCaptureMouse);
  }

  void WorldScene::update(float dt) {
    for (const auto& event : eventQueue_.getEvents<event::TogglePanel>()) {
      uiManager_.getPanel(event.name).toggleVisible();
    }

    for (const auto& event : eventQueue_.getEvents<event::SetPanelVisibility>()) {
      uiManager_.getPanel(event.name).setVisible(event.visible);
    }

    UpdateContext ctx{
      .registry = registry_,
      .eventQueue = eventQueue_,
      .textureManager = textureManager_,
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto entity : registry_.view<component::Camera>()) {
      const auto& camera{ registry_.getComponent<component::Camera>(entity) };
      glm::mat4 viewProjection{ camera.projection * camera.view };
      renderPipeline_.execute(
          renderer::RenderContext{
              .registry = registry_,
              .viewProjection = viewProjection,
              .textureManager = textureManager_,
          }
      );
      break;
    }

    uiManager_.render(getUIContext());
  }

}  // namespace ls
