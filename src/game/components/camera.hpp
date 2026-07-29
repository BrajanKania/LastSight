#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  struct Camera {
    float orthographicSize{10.f};
    float zoom{1.f};

    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
  };

}  // namespace ls::component
