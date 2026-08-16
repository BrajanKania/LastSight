#pragma once

#include <glm/ext/vector_float2.hpp>

namespace ls::event {

  struct ViewportResized {
    glm::vec2 newSize{ 0.f };
  };

}  // namespace ls::event
