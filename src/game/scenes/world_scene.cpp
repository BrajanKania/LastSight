#include "game/scenes/world_scene.hpp"

#include <SDL3/SDL_log.h>
#include <glad/gl.h>

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "engine/components/collider.hpp"
#include "engine/components/entity_name.hpp"
#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/core/update_context.hpp"
#include "engine/gfx/framebuffer.hpp"
#include "engine/physics/physics_system.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/layer.hpp"
#include "engine/renderer/passes/compose_pass.hpp"
#include "engine/renderer/passes/fov_pass.hpp"
#include "engine/renderer/passes/lit_pass.hpp"
#include "engine/renderer/passes/post_process_pass.hpp"
#include "game/components/camera.hpp"
#include "game/components/field_of_view.hpp"
#include "game/components/movement.hpp"
#include "game/components/player.hpp"
#include "game/components/weapon.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/combat_system.hpp"
#include "game/systems/player_system.hpp"
#include "game/systems/projectile_system.hpp"
#include "game/ui/entity_explorer_panel.hpp"
#include "game/ui/render_pipeline_debug_panel.hpp"

namespace ls {

  void WorldScene::onEnter() {
    uint32_t playerTextureId{ textureManager_.load("player", asset_system::texture("player.png")) };
    uint32_t backgroundTextureId{ textureManager_.load("grass", asset_system::texture("grass.jpg")) };
    uint32_t containerTextureId{ textureManager_.load("container", asset_system::texture("container.png")) };
    textureManager_.load("bullet", asset_system::texture("bullet.png"));

    // background
    {
      auto background{ registry_.createEntity() };
      registry_.addComponent(background, component::Transform{ .scale = glm::vec2(5.f) });
      registry_.addComponent(
          background,
          component::Sprite{
              .uvScale = glm::vec2(2.5f),
              .textureId = backgroundTextureId,
              .zIndex = renderer::Layer::Background,
          }
      );
    }

    // player
    {
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
              .textureId = playerTextureId,
              .angleOffset = -90.f,
              .zIndex = renderer::Layer::Ground,
          }
      );
      registry_.addComponent(player_, component::Velocity{});
      registry_.addComponent(
          player_,
          component::Movement{
              .runSpeed = 2.f,
          }
      );
      registry_.addComponent(
          player_,
          component::FieldOfView{
              .innerRadius = 1.0f,
              .outerRadius = 6.f,
              .fovAngle = 100.f,
              .smoothnessAngle = 40.f,
              .smoothnessDistance = 2.f,
          }
      );
      registry_.addComponent(
          player_,
          component::Collider{
              .type = component::ColliderType::Circle,
              .radius = 0.3f,
          }
      );
      registry_.addComponent(
          player_,
          component::Weapon{
              .fireRate = 0.15f,
              .initialSpeed = 4.f,
              .barrelOffset = glm::vec2(-0.07f, 0.22f),
              .bulletScale = glm::vec2(0.02f),
              .bulletLifetime = 5.f,
          }
      );
    }

    // camera
    {
      auto camera{ registry_.createEntity() };
      registry_.addComponent(
          camera,
          component::Camera{
              .orthographicSize = 4.f,
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
    }

    // container
    {
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
              .textureId = containerTextureId,
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
    }

    // enemy
    {
      auto enemy{ registry_.createEntity() };
      registry_.addComponent(
          enemy,
          component::Transform{
              .scale = glm::vec2(0.5f),
              .position = glm::vec2(-1.f),
          }
      );
      registry_.addComponent(
          enemy,
          component::Sprite{
              .uvScale = glm::vec2(1.f),
              .textureId = playerTextureId,
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

    // container 2
    {
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
              .textureId = containerTextureId,
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

    worldFBO_ = std::make_shared<gfx::Framebuffer>();
    fovFBO_ = std::make_shared<gfx::Framebuffer>();
    processedFBO_ = std::make_shared<gfx::Framebuffer>();

    renderPipeline_.addPass<renderer::LitPass>(worldFBO_);
    renderPipeline_.addPass<renderer::FovPass>(fovFBO_, worldFBO_);
    renderPipeline_.addPass<renderer::PostProcessPass>(processedFBO_, fovFBO_);
    renderPipeline_.addPass<renderer::ComposePass>(processedFBO_);

    uiManager_.addPanel<ui::RenderPipelineDebugPanel>("render_pipeline_debbuger", renderPipeline_);
    uiManager_.addPanel<ui::EntityExplorerPanel>("entity_explorer_panel");
  }

  void WorldScene::onExit() {}

  void WorldScene::onResize(int width, int height) {
    worldFBO_->resize(width, height);
    fovFBO_->resize(width, height);
    processedFBO_->resize(width, height);
  }

  void WorldScene::handleInput() {}

  void WorldScene::update(float dt) {
    UpdateContext ctx{
      .registry = registry_,
      .eventQueue = eventQueue_,
      .textureManager = textureManager_,
      .dt = dt,
    };

    player_system::update(ctx);
    combat_system::update(ctx);
    physics_system::update(ctx);
    projectile_system::update(ctx);

    camera_system::follow(ctx, player_, 6.f);
    camera_system::update(ctx);

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
