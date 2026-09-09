#include "ls/extraction/extraction_system.hpp"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>

#include "ls/components/sprite.hpp"
#include "ls/components/transform.hpp"
#include "ls/renderer/commands/draw_sprite.hpp"

namespace ls::extraction_system {

  void extractRenderCommands(renderer::CommandBuffer& cmdBuffer, ecs::Registry& registry) {
    for (auto entity : registry.view<component::Sprite>()) {
      cmdBuffer.push(extractDrawSpriteCommand(registry, entity));
    }
  }

  renderer::command::DrawSprite extractDrawSpriteCommand(ecs::Registry& registry, ecs::EntityId entity) {
    const auto& sprite{ registry.getComponent<component::Sprite>(entity) };

    glm::mat4 model{ 1.f };
    if (registry.hasComponent<component::Transform>(entity)) {
      const auto& transform{ registry.getComponent<component::Transform>(entity) };
      model = glm::translate(model, glm::vec3{ transform.position.x, transform.position.y, 0.f });
      model = glm::rotate(model, glm::radians(transform.rotation + sprite.angleOffset), glm::vec3{ 0.f, 0.f, 1.f });
      model = glm::scale(model, glm::vec3{ transform.scale.x, transform.scale.y, 1.f });
    }

    return renderer::command::DrawSprite{
      .materialHandle = sprite.materialHandle,
      .textureHandle = sprite.textureHandle,
      .layer = sprite.layer,
      .color = sprite.color,
      .uvScale = sprite.uvScale,
      .model = model,
    };
  }

}  // namespace ls::extraction_system
