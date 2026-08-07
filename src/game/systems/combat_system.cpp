#include "game/systems/combat_system.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <random>

#include "engine/components/equipped_sprite.hpp"
#include "engine/components/particle_emitter.hpp"
#include "engine/components/transform.hpp"
#include "engine/core/update_context.hpp"
#include "engine/ecs/types.hpp"
#include "engine/gfx/texture_manager.hpp"
#include "game/components/camera_shake.hpp"
#include "game/components/weapon.hpp"
#include "game/events/request_shoot.hpp"
#include "game/factories/projectile_factory.hpp"
#include "game/particles/gun_sparks.hpp"
#include "game/scenes/texture_names.hpp"
#include "game/systems/camera_system.hpp"

namespace ls::combat_system {

  namespace {

    float getRangeRandom(float min, float max) {
      static std::mt19937 gen{ std::random_device{}() };
      assert(min < max);
      std::uniform_real_distribution<float> dist{ min, max };
      return dist(gen);
    }

    void shoot(const UpdateContext& ctx, const ecs::EntityId shooterEntity) {
      const auto& transform{ ctx.registry.getComponent<component::Transform>(shooterEntity) };
      auto& weapon{ ctx.registry.getComponent<component::Weapon>(shooterEntity) };
      const auto& recoil{ weapon.isAiming ? weapon.aimRecoil : weapon.hipRecoil };

      weapon.springVelocity.y -= recoil.weaponImpulse * getRangeRandom(0.8f, 1.f);
      weapon.springAngularVelocity += getRangeRandom(-recoil.weaponAngularImpulse, recoil.weaponAngularImpulse);

      float spreadAngle{ getRangeRandom(-recoil.baseSpread * 0.5f, recoil.baseSpread * 0.5f) };
      float totalAngleDeg{ transform.rotation + weapon.springRotation + spreadAngle };
      float totalAngleRad{ glm::radians(totalAngleDeg) };

      glm::vec2 bulletDirection{ glm::cos(totalAngleRad), glm::sin(totalAngleRad) };

      float baseAngleRad{ glm::radians(transform.rotation + weapon.springRotation) };
      glm::vec2 forward{ glm::cos(baseAngleRad), glm::sin(baseAngleRad) };
      glm::vec2 right{ -forward.y, forward.x };

      glm::vec2 totalMuzzleOffset{ weapon.offset + weapon.springOffset + weapon.muzzleOffset };
      glm::vec2 rotatedMuzzleOffset{ forward * totalMuzzleOffset.y - right * totalMuzzleOffset.x };
      glm::vec2 muzzleWorldPos{ transform.position + rotatedMuzzleOffset };

      factory::ProjectileConfig config{
        .scale = weapon.bulletScale,
        .position = muzzleWorldPos,
        .direction = bulletDirection,
        .speed = weapon.initialSpeed,
        .textureId = ctx.textureManager.getId(texture_name::kBullet),
        .angleOffset = -90.f,
        .lifetime = weapon.bulletLifetime,
      };

      factory::spawnProjectile(ctx.registry, config);

      ecs::EntityId particleEmitter{ ctx.registry.createEntity() };
      ctx.registry.addComponent(particleEmitter, component::ParticleEmitter{ particle::preset::gunSparks() });
      ctx.registry.addComponent(
          particleEmitter,
          component::Transform{
              .position = muzzleWorldPos,
          }
      );

      for (ecs::EntityId cameraEntity : ctx.registry.view<component::Camera, component::CameraShake>()) {
        camera_system::addImpulse(ctx.registry, cameraEntity, -bulletDirection * recoil.cameraImpulse);
        if (recoil.cameraTrauma > 0.f) {
          camera_system::addTrauma(ctx.registry, cameraEntity, recoil.cameraTrauma);
        }
      }

      weapon.cooldown = weapon.fireRate;
    }

    void updateEquippedWeaponModel(const UpdateContext& ctx, const ecs::EntityId weaponEntity) {
      auto& weapon{ ctx.registry.getComponent<component::Weapon>(weaponEntity) };
      auto& equippedSprite{ ctx.registry.getComponent<component::EquippedSprite>(weaponEntity) };
      const auto& recoil{ weapon.isAiming ? weapon.aimRecoil : weapon.hipRecoil };

      glm::vec2 targetOffset{ weapon.isAiming ? weapon.aimOffset : weapon.hipOffset };
      float factor{ glm::clamp(weapon.transitionSpeed * ctx.dt, 0.f, 1.f) };
      weapon.offset = glm::mix(weapon.offset, targetOffset, factor);

      glm::vec2 springForce{ -recoil.springStiffness * weapon.springOffset - recoil.damping * weapon.springVelocity };
      weapon.springVelocity += springForce * ctx.dt;
      weapon.springOffset += weapon.springVelocity * ctx.dt;
      if (glm::length(weapon.springOffset) > recoil.maxSpringOffset) {
        weapon.springOffset = glm::normalize(weapon.springOffset) * recoil.maxSpringOffset;
      }

      float angularForce{ -recoil.angularSpringStiffness * weapon.springRotation -
                          recoil.angularDamping * weapon.springAngularVelocity };
      weapon.springAngularVelocity += angularForce * ctx.dt;
      weapon.springRotation += weapon.springAngularVelocity * ctx.dt;

      weapon.springRotation = glm::clamp(weapon.springRotation, -recoil.maxSpringRotation, recoil.maxSpringRotation);

      equippedSprite.offset = weapon.offset + weapon.springOffset;
      equippedSprite.angleOffset = weapon.baseAngleOffset + weapon.springRotation;
    }

  }  // namespace

  void update(const UpdateContext& ctx) {
    for (auto entity : ctx.registry.view<component::Weapon>()) {
      auto& weapon{ ctx.registry.getComponent<component::Weapon>(entity) };
      if (weapon.cooldown > 0.f) {
        weapon.cooldown -= ctx.dt;
      }
    }

    for (auto event : ctx.eventQueue.getEvents<event::RequestShoot>()) {
      if (ctx.registry.hasComponent<component::Weapon>(event.shooter)) {
        const auto& weapon{ ctx.registry.getComponent<component::Weapon>(event.shooter) };
        if (weapon.cooldown <= 0.f) {
          shoot(ctx, event.shooter);
        }
      }
    }

    for (auto entity : ctx.registry.view<component::Weapon, component::EquippedSprite>()) {
      updateEquippedWeaponModel(ctx, entity);
    }
  }

}  // namespace ls::combat_system
