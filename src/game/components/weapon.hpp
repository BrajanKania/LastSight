#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>

#include "engine/reflection/reflection_macros.hpp"

namespace ls::component {

  COMPONENT()
  struct Weapon {
    STRUCT()
    struct RecoilConfig {
      PROPERTY()
      float baseSpread{ 2.5f };

      PROPERTY()
      float weaponImpulse{ 10.f };

      PROPERTY()
      float weaponAngularImpulse{ 20.f };

      PROPERTY()
      float cameraImpulse{ 5.f };

      PROPERTY()
      float cameraTrauma{ 1.f };

      PROPERTY()
      float maxSpringOffset{ 1.f };

      PROPERTY()
      float springStiffness{ 400.f };

      PROPERTY()
      float damping{ 20.f };

      PROPERTY()
      float maxSpringRotation{ 12.f };

      PROPERTY()
      float angularSpringStiffness{ 20.f };

      PROPERTY()
      float angularDamping{ 12.f };
    };

    PROPERTY()
    bool isAutomatic{ false };

    PROPERTY(ReadOnly, Transient)
    bool isAiming{ false };

    PROPERTY()
    float fireRate{ 0.5f };

    PROPERTY(ReadOnly, Transient)
    float cooldown{ 0.f };

    PROPERTY()
    float initialSpeed{ 5.f };

    PROPERTY()
    glm::vec2 bulletScale{ 1.f };

    PROPERTY()
    float bulletLifetime{ 1.f };

    PROPERTY()
    float transitionSpeed{ 1.f };

    PROPERTY()
    float baseAngleOffset{ 0.f };

    PROPERTY(ReadOnly, Transient)
    glm::vec2 offset{ 0.f };

    PROPERTY(ReadOnly)
    glm::vec2 muzzleOffset{ 0.f };

    PROPERTY(ReadOnly)
    glm::vec2 hipOffset{ 0.f };

    PROPERTY(ReadOnly)
    glm::vec2 aimOffset{ 0.f };

    PROPERTY(ReadOnly, Transient)
    glm::vec2 springOffset{ 0.f };

    PROPERTY(ReadOnly, Transient)
    glm::vec2 springVelocity{ 0.f };

    PROPERTY(ReadOnly, Transient)
    float springRotation{ 0.f };

    PROPERTY(ReadOnly, Transient)
    float springAngularVelocity{ 0.f };

    PROPERTY(DisplayName("Hip recoil config"))
    RecoilConfig hipRecoil;

    PROPERTY(DisplayName("Aim recoil config"))
    RecoilConfig aimRecoil;
  };

}  // namespace ls::component
