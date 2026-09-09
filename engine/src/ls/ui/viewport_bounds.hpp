#pragma once

#include <glm/ext/vector_float2.hpp>

namespace ls::ui {

  struct ViewportBounds {
    glm::vec2 position{ 0.f };
    glm::vec2 size{ 0.f };
  };

}  // namespace ls::ui
