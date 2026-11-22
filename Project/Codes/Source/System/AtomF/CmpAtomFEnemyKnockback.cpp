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