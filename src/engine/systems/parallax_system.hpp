#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/core/update_context.hpp"

namespace ls::parallax_system {

  void update(const UpdateContext& ctx, glm::vec2 cameraOffset);

}
