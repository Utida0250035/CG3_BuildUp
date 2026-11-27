#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/AtomF/CmpAtomFEnemyTargetedPanic.h"
#include "System/AtomF/CmpAtomFEnemyVariant.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Particle/GameCombatParticleEffects.h"

#include <cmath>

namespace Atrum {

void CmpAtomFEnemyPeriodicCharge::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  chargeIntervalSeconds_ = AtomF::ResolveEnemyChargeIntervalSeconds();
  chargeTiming_ = AtomF::ResolveEnemyPeriodicChargeTimingParameters();
  if (const CmpAtomFEnemyVariant *variant =
          RefOwner().GetUpdCmp<CmpAtomFEnemyVariant>();
      variant != nullptr) {
    const AtomF::EnemyVariantParameters &v = variant->Parameters();
    chargeEnabled_ = v.chargeEnabled;
    if (v.chargeIntervalSeconds > 0.0f) {
      chargeIntervalSeconds_ = v.chargeIntervalSeconds;
    }
  }
}

void CmpAtomFEnemyPeriodicCharge::BeginShaking() {
  phase_ = Phase::Shaking;
  phaseElapsedSeconds_ = 0.0f;
  anchorTranslate_ = RefOwner().RefTransform().translate;
  // 予備動作: 突進の溜めを火の粉で見せる。
  Particle::Combat::PlayOneShotEffectByFileName(
      "atomf_enemy_charge_windup.json", RefOwner().GetWorldPosition());
}

void CmpAtomFEnemyPeriodicCharge::BeginCharging() {
  phase_ = Phase::Charging;
  phaseElapsedSeconds_ = 0.0f;
  anchorTranslate_ = RefOwner().RefTransform().translate;
  Particle::Combat::PlayOneShotEffectByFileName(
      "atomf_enemy_charge_windup.json", RefOwner().GetWorldPosition());
}

void CmpAtomFEnemyPeriodicCharge::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      return;
    }
  }

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    if (CmpAtomFEnemyKnockback *knockback =
            RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
        knockback != nullptr && knockback->HasActiveKnockback()) {
      if (phase_ != Phase::Waiting) {
        phase_ = Phase::Waiting;
        waitTimerSeconds_ = 0.0f;
      }
      return;
    }
  }

  if (CmpAtomFEnemyTargetedPanic *panic =
          RefOwner().GetUpdCmp<CmpAtomFEnemyTargetedPanic>();
      panic != nullptr && panic->BlocksLocomotion()) {
    return;
  }

  const float dt = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Waiting) {
    if (!chargeEnabled_) {
      return;
    }
    waitTimerSeconds_ += dt;
    if (waitTimerSeconds_ >= chargeIntervalSeconds_) {
      waitTimerSeconds_ = 0.0f;
      BeginShaking();
    }
    return;
  }

  phaseElapsedSeconds_ += dt;

  if (phase_ == Phase::Shaking) {
    const float wobble =
        std::sin(phaseElapsedSeconds_ * chargeTiming_.shakeAngularSpeed);
    Math::Vector3 &translate = RefOwner().RefTransform().translate;
    translate = anchorTranslate_;
    translate.x += wobble * chargeTiming_.shakeAmplitudeMeters;
    translate.z +=
        std::cos(phaseElapsedSeconds_ * chargeTiming_.shakeAngularSpeed *
                 1.3f) *
        (chargeTiming_.shakeAmplitudeMeters * 0.7f);

    if (phaseElapsedSeconds_ >= chargeTiming_.shakeDurationSeconds) {
      BeginCharging();
    }
    return;
  }

  if (phase_ == Phase::Charging) {
    Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
    if (player != nullptr && player->IsActive()) {
      const Math::Vector3 playerPos = player->GetWorldPosition();
      Math::Vector3 &translate = RefOwner().RefTransform().translate;
      Math::Vector3 toPlayer = playerPos - translate;
      toPlayer.y = 0.0f;
      const float distSq = toPlayer.LengthSquare();
      if (distSq > 1e-6f) {
        toPlayer *= (1.0f / std::sqrt(distSq));
        const float speed =
            AtomF::ResolveEnemyKnockbackSpeedMetersPerSecond();
        translate += toPlayer * speed * dt;
      }
    }

    if (phaseElapsedSeconds_ >= chargeTiming_.chargeDurationSeconds) {
      phase_ = Phase::Waiting;
      phaseElapsedSeconds_ = 0.0f;
    }
  }
}

} // namespace Atrum