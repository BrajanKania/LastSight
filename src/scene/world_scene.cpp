#include "world_scene.hpp"

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/asset_system.hpp"
#include "core/component.hpp"
#include "renderer/renderer_system.hpp"

namespace ls {

  void WorldScene::onEnter() {
    shader_.emplace(asset_system::shader("vertex.glsl"), asset_system::shader("fragment.glsl"));

    // clang-format off
    float vertex[] {
      -0.5f, -0.5f,
      0.5f, -0.5f,
      0.5f, 0.5f,

      -0.5f, -0.5f,
      0.5f, 0.5f,
      -0.5f, 0.5f
    };
    // clang-format on

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    auto background{registry_.createEntity()};
    registry_.addComponent(background, ls::component::Transform{.scale = glm::vec3(1.f)});
    registry_.addComponent(background, ls::component::Sprite{.color = {0.2f, 1.f, 0.2f}});

    auto player{registry_.createEntity()};
    registry_.addComponent(player, ls::component::Transform{.scale = glm::vec3(0.1f)});
    registry_.addComponent(player, ls::component::Sprite{.color = {0.2f, 0.2f, 1.0f}});
  }

  void WorldScene::onExit() {}

  void WorldScene::handleInput() {}

  void WorldScene::update(float dt) {}

  void WorldScene::render() {
    ls::renderer_system::useFramebuffer(0);
    ls::renderer_system::clearColorBuffer();

    shader_->use();

    for (auto entity : registry_.view<ls::component::Transform, ls::component::Sprite>()) {
      const auto& transform{registry_.getComponent<ls::component::Transform>(entity)};
      const auto& sprite{registry_.getComponent<ls::component::Sprite>(entity)};

      glm::mat4 model{1.f};
      model = glm::translate(model, transform.position);
      model = glm::scale(model, transform.scale);
      shader_->setMat4("uModel", model);
      shader_->setVec3("uColor", sprite.color);
      ls::renderer_system::drawArrays(vao_, ls::renderer_system::Primitive::Triangle, 0, 6);
    }
  }

}  // namespace ls
