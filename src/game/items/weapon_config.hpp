#pragma once

#include <glm/ext/vector_float2.hpp>
namespace ls::item {

  struct WeaponConfig {
    bool isAutomatic{ false };
    float fireRate{ 1.f };
    float initialSpeed{ 5.f };
    glm::vec2 bulletScale{ 0.05f };
    float bulletLifetime{ 3.f };
    glm::vec2 barrelOffset{ 0.f };
    float recoilImpulse{ 0.f };
    float recoilTrauma{ 0.f };
  };

}  // namespace ls::item
