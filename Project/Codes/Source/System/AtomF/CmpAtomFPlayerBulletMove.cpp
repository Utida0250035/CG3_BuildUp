#include "System/AtomF/CmpAtomFPlayerBulletMove.h"

#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpAtomFPlayerBulletMove::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFPlayerBulletMove::SetDirection(const Math::Vector3 &directionXZUnit) {
  direction_ = directionXZUnit;
  direction_.y = 0.0f;
  if (direction_.LengthSquare() > 1e-6f) {
    direction_.Normalize();
  } else {
    direction_ = {0.0f, 0.0f, 1.0f};
  }
}

void CmpAtomFPlayerBulletMove::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  float speed = 1.5f;
  const AtomF::ParametersRuntime &params = AtomF::ParametersRuntime::Instance();
  if (params.IsLoaded()) {
    speed = params.Player().bullet.speedMetersPerSecond;
  }

  RefOwner().RefTransform().translate +=
      direction_ * speed * frameDeltaTime_->GetDeltaTime();
}

} // namespace Atrum