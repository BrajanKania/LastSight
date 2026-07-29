#include "game/scenes/world_scene.hpp"

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/core/asset_system.hpp"
#include "engine/physics/physics_system.hpp"
#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/passes/compose_pass.hpp"
#include "engine/renderer/passes/lit_pass.hpp"
#include "engine/renderer/passes/post_process_pass.hpp"
#include "game/components/camera.hpp"
#include "game/components/movement.hpp"
#include "game/components/player.hpp"
#include "game/systems/camera_system.hpp"
#include "game/systems/player_system.hpp"

namespace ls {

  void WorldScene::onEnter() {
    uint32_t playerTextureId{textureManager_.load("player", asset_system::texture("player.png"))};
    uint32_t backgroundTextureId{textureManager_.load("grass", asset_system::texture("grass.jpg"))};

    auto background{registry_.createEntity()};
    registry_.addComponent(background, component::Transform{.scale = glm::vec2(5.f)});
    registry_.addComponent(background,
                           component::Sprite{.color = {1.f, 1.f, 1.f, 1.f}, .textureId = backgroundTextureId});

    player_ = registry_.createEntity();
    registry_.addComponent(player_, component::Player{});
    registry_.addComponent(player_, component::Transform{.scale = glm::vec2(0.5f)});
    registry_.addComponent(player_, component::Sprite{.color = {1.f, 1.f, 1.f, 1.f}, .textureId = playerTextureId});
    registry_.addComponent(player_, component::Velocity{});
    registry_.addComponent(player_, component::Movement{});

    auto camera{registry_.createEntity()};
    registry_.addComponent(camera, component::Camera{.orthographicSize = 10.f, .zoom = 1.f});
    registry_.addComponent(camera, component::Transform{});

    worldFBO_ = std::make_shared<Framebuffer>(1000, 800);
    processedFBO_ = std::make_shared<Framebuffer>(1000, 800);

    renderPipeline_.addPass<LitPass>(worldFBO_);
    renderPipeline_.addPass<PostProcessPass>(processedFBO_, worldFBO_);
    renderPipeline_.addPass<ComposePass>(processedFBO_);
  }

  void WorldScene::onExit() {}

  void WorldScene::onResize(int width, int height) {
    worldFBO_->resize(width, height);
    processedFBO_->resize(width, height);
  }

  void WorldScene::handleInput() {}

  void WorldScene::update(float dt) {
    player_system::update(registry_, dt);
    physics_system::update(registry_, dt);

    camera_system::follow(registry_, player_, dt, 10.f);
    camera_system::update(registry_);
  }

  void WorldScene::render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto entity : registry_.view<component::Camera>()) {
      const auto& camera{registry_.getComponent<component::Camera>(entity)};
      glm::mat4 viewProjection{camera.projection * camera.view};
      renderPipeline_.execute(
          RenderContext{.registry = registry_, .viewProjection = viewProjection, .textureManager = textureManager_});
      break;
    }
  }

}  // namespace ls
