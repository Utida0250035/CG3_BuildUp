#include "System/AtomF/CmpAtomFEnemyPeriodicCharge.h"

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kShakeDurationSeconds = 0.45f;
constexpr float kChargeDurationSeconds = 0.75f;
constexpr float kShakeAmplitudeMeters = 0.1f;
constexpr float kShakeAngularSpeed = 48.0f;

} // namespace

void CmpAtomFEnemyPeriodicCharge::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
  chargeIntervalSeconds_ = AtomF::ResolveEnemyChargeIntervalSeconds();
}

void CmpAtomFEnemyPeriodicCharge::BeginShaking() {
  phase_ = Phase::Shaking;
  phaseElapsedSeconds_ = 0.0f;
  anchorTranslate_ = RefOwner().RefTransform().translate;
}

void CmpAtomFEnemyPeriodicCharge::BeginCharging() {
  phase_ = Phase::Charging;
  phaseElapsedSeconds_ = 0.0f;
  anchorTranslate_ = RefOwner().RefTransform().translate;
}

void CmpAtomFEnemyPeriodicCharge::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  if (CmpAtomFEnemyKnockback *knockback =
          RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
      knockback != nullptr && knockback->HasActiveKnockback()) {
    if (phase_ != Phase::Waiting) {
      phase_ = Phase::Waiting;
      waitTimerSeconds_ = 0.0f;
    }
    return;
  }

  const float dt = frameDeltaTime_->GetDeltaTime();

  if (phase_ == Phase::Waiting) {
    waitTimerSeconds_ += dt;
    if (waitTimerSeconds_ >= chargeIntervalSeconds_) {
      waitTimerSeconds_ = 0.0f;
      BeginShaking();
    }
    return;
  }

  phaseElapsedSeconds_ += dt;

  if (phase_ == Phase::Shaking) {
    const float wobble = std::sin(phaseElapsedSeconds_ * kShakeAngularSpeed);
    Math::Vector3 &translate = RefOwner().RefTransform().translate;
    translate = anchorTranslate_;
    translate.x += wobble * kShakeAmplitudeMeters;
    translate.z += std::cos(phaseElapsedSeconds_ * kShakeAngularSpeed * 1.3f) *
                     kShakeAmplitudeMeters * 0.7f;

    if (phaseElapsedSeconds_ >= kShakeDurationSeconds) {
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

    if (phaseElapsedSeconds_ >= kChargeDurationSeconds) {
      phase_ = Phase::Waiting;
      phaseElapsedSeconds_ = 0.0f;
    }
  }
}

} // namespace Atrum