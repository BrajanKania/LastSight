#pragma once

#include <glm/glm.hpp>

namespace ls::component {

  struct Weapon {
    bool isAutomatic{ false };
    float fireRate{ 0.5f };
    float cooldown{ 0.f };
    float initialSpeed{ 5.f };
    glm::vec2 barrelOffset{ 0.f };
    glm::vec2 bulletScale{ 1.f };
    float bulletLifetime{ 1.f };

    float recoilImpulse{ 0.f };
    float recoilTrauma{ 0.f };
  };

}  // namespace ls::component
