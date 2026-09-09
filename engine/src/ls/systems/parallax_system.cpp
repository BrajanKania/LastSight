#include "ls/systems/parallax_system.hpp"

#include <glm/ext/vector_float2.hpp>

#include "ls/components/parallax.hpp"
#include "ls/components/transform.hpp"

namespace ls::parallax_system {

  void update(const UpdateContext& ctx, glm::vec2 cameraOffset) {
    auto view = ctx.registry.view<component::Transform, component::Parallax>();
    for (auto entity : ctx.registry.view<component::Transform, component::Parallax>()) {
      auto& transform{ ctx.registry.getComponent<component::Transform>(entity) };
      const auto& parallax{ ctx.registry.getComponent<component::Parallax>(entity) };

      transform.position = parallax.basePosition + (cameraOffset * parallax.factor);
    }
  }

}  // namespace ls::parallax_system
