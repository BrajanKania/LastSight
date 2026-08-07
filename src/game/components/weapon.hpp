#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

namespace ls::component {

  struct Weapon {
    struct RecoilConfig {
      float baseSpread{ 2.5f };

      float weaponImpulse{ 10.f };
      float weaponAngularImpulse{ 20.f };
      float cameraImpulse{ 5.f };
      float cameraTrauma{ 1.f };

      float maxSpringOffset{ 1.f };
      float springStiffness{ 400.f };
      float damping{ 20.f };

      float maxSpringRotation{ 12.f };
      float angularSpringStiffness{ 20.f };
      float angularDamping{ 12.f };
    };

    bool isAutomatic{ false };
    bool isAiming{ false };

    float fireRate{ 0.5f };
    float cooldown{ 0.f };

    float initialSpeed{ 5.f };
    glm::vec2 bulletScale{ 1.f };
    float bulletLifetime{ 1.f };

    float transitionSpeed{ 1.f };
    float baseAngleOffset{ 0.f };

    glm::vec2 offset{ 0.f };

    glm::vec2 muzzleOffset{ 0.f };
    glm::vec2 hipOffset{ 0.f };
    glm::vec2 aimOffset{ 0.f };

    glm::vec2 springOffset{ 0.f };
    glm::vec2 springVelocity{ 0.f };

    float springRotation{ 0.f };
    float springAngularVelocity{ 0.f };

    RecoilConfig hipRecoil;
    RecoilConfig aimRecoil;
  };

}  // namespace ls::component
