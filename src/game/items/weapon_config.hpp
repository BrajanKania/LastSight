#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/reflection/reflection_macros.hpp"
#include "game/components/weapon.hpp"

namespace ls::item {

  STRUCT()
  struct WeaponConfig {
    PROPERTY()
    bool isAutomatic{ false };

    PROPERTY()
    float fireRate{ 1.f };

    PROPERTY()
    float initialSpeed{ 5.f };

    PROPERTY()
    glm::vec2 bulletScale{ 0.05f };

    PROPERTY()
    float bulletLifetime{ 3.f };

    PROPERTY()
    float transitionSpeed{ 1.f };

    PROPERTY()
    glm::vec2 muzzleOffset{ 0.f };

    PROPERTY()
    glm::vec2 hipOffset{ 0.f };

    PROPERTY()
    glm::vec2 aimOffset{ 0.f };

    PROPERTY()
    component::Weapon::RecoilConfig hipRecoil;

    PROPERTY()
    component::Weapon::RecoilConfig aimRecoil;
  };

}  // namespace ls::item
