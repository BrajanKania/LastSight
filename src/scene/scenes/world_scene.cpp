#include "world_scene.hpp"

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "core/component.hpp"
#include "renderer/framebuffer.hpp"
#include "renderer/render_pass.hpp"
#include "renderer/render_passes/compose_pass.hpp"
#include "renderer/render_passes/lit_pass.hpp"
#include "renderer/render_passes/post_process_pass.hpp"

namespace ls {

  void WorldScene::onEnter() {
    auto background{registry_.createEntity()};
    registry_.addComponent(background, ls::component::Transform{.scale = glm::vec3(1.f)});
    registry_.addComponent(background, ls::component::Sprite{.color = {0.2f, 1.f, 0.2f}});

    auto player{registry_.createEntity()};
    registry_.addComponent(player, ls::component::Transform{.scale = glm::vec3(0.1f)});
    registry_.addComponent(player, ls::component::Sprite{.color = {0.2f, 0.2f, 1.0f}});

    auto worldFBO{std::make_shared<Framebuffer>(1000, 800)};
    auto processedFBO{std::make_shared<Framebuffer>(1000, 800)};

    renderPipeline_.addPass<LitPass>(worldFBO);
    renderPipeline_.addPass<PostProcessPass>(processedFBO, worldFBO);
    renderPipeline_.addPass<ComposePass>(processedFBO);
  }

  void WorldScene::onExit() {}

  void WorldScene::handleInput() {}

  void WorldScene::update(float dt) {}

  void WorldScene::render() { renderPipeline_.execute(RenderContext{.registry = registry_}); }

}  // namespace ls
