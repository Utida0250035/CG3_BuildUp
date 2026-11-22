#include "System/AtomF/CmpAtomFEnemyKnockback.h"

#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFEnemyKnockback::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFEnemyKnockback::ApplyKnockbackAlongBulletDirection(
    const Math::Vector3 &directionXZUnit, const float speedMetersPerSecond) {
  velocity_ = directionXZUnit;
  velocity_.y = 0.0f;
  if (velocity_.LengthSquare() > 1e-6f) {
    velocity_.Normalize();
  } else {
    velocity_ = {0.0f, 0.0f, 1.0f};
  }
  velocity_ *= speedMetersPerSecond;
  chainImpactKnockback_ = true;
}

void CmpAtomFEnemyKnockback::ApplyKnockbackAwayFromExplosionCenter(
    const Math::Vector3 &explosionCenterWorld,
    const float speedMetersPerSecond) {
  Math::Vector3 away = RefOwner().GetWorldPosition() - explosionCenterWorld;
  away.y = 0.0f;
  ApplyKnockbackAlongBulletDirection(away, speedMetersPerSecond);
}

void CmpAtomFEnemyKnockback::ClearKnockbackVelocity() {
  velocity_ = {};
  chainImpactKnockback_ = false;
}

void CmpAtomFEnemyKnockback::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  if (velocity_.LengthSquare() < 1e-8f) {
    return;
  }

  RefOwner().RefTransform().translate +=
      velocity_ * frameDeltaTime_->GetDeltaTime();
}

} // namespace Atrum