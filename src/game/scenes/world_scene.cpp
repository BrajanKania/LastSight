#include "game/scenes/world_scene.hpp"

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "engine/components/sprite.hpp"
#include "engine/components/transform.hpp"
#include "engine/components/velocity.hpp"
#include "engine/physics/physics_system.hpp"
#include "engine/renderer/framebuffer.hpp"
#include "engine/renderer/i_render_pass.hpp"
#include "engine/renderer/passes/compose_pass.hpp"
#include "engine/renderer/passes/lit_pass.hpp"
#include "engine/renderer/passes/post_process_pass.hpp"
#include "game/components/movement.hpp"
#include "game/components/player.hpp"
#include "game/systems/player_system.hpp"

namespace ls {

  void WorldScene::onEnter() {
    auto background{registry_.createEntity()};
    registry_.addComponent(background, component::Transform{.scale = glm::vec2(1.f)});
    registry_.addComponent(background, component::Sprite{.color = {0.2f, 1.f, 0.2f}});

    auto player{registry_.createEntity()};
    registry_.addComponent(player, component::Player{});
    registry_.addComponent(player, component::Transform{.scale = glm::vec2(0.1f)});
    registry_.addComponent(player, component::Sprite{.color = {0.2f, 0.2f, 1.0f}});
    registry_.addComponent(player, component::Velocity{});
    registry_.addComponent(player, component::Movement{});

    auto worldFBO{std::make_shared<Framebuffer>(1000, 800)};
    auto processedFBO{std::make_shared<Framebuffer>(1000, 800)};

    renderPipeline_.addPass<LitPass>(worldFBO);
    renderPipeline_.addPass<PostProcessPass>(processedFBO, worldFBO);
    renderPipeline_.addPass<ComposePass>(processedFBO);
  }

  void WorldScene::onExit() {}

  void WorldScene::handleInput() {}

  void WorldScene::update(float dt) {
    player_system::update(registry_, dt);
    physics_system::update(registry_, dt);
  }

  void WorldScene::render() { renderPipeline_.execute(RenderContext{.registry = registry_}); }

}  // namespace ls
