#pragma once

#include <glm/ext/vector_float2.hpp>

#include "engine/reflection/reflection_macros.hpp"
#include "game/components/weapon.hpp"

namespace ls::item {

  STRUCT()
  struct WeaponConfig {
    bool isAutomatic{ false };

    float fireRate{ 1.f };

    float initialSpeed{ 5.f };
    glm::vec2 bulletScale{ 0.05f };
    float bulletLifetime{ 3.f };

    float transitionSpeed{ 1.f };

    glm::vec2 muzzleOffset{ 0.f };
    glm::vec2 hipOffset{ 0.f };
    glm::vec2 aimOffset{ 0.f };

    component::Weapon::RecoilConfig hipRecoil;
    component::Weapon::RecoilConfig aimRecoil;
  };

}  // namespace ls::item
