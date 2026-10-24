#include "System/CmpSwing.h"
#include "Math/Quaternion.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpSwing::Update() {
  if (swingTimer_ <= 0.0f || !pivot_) {
    return;
  }

  Math::Quaternion &quaternion = pivot_->RefTransform().quaternion;
  quaternion = Math::Quaternion::FromAxisAngle(swingAxis_, swingSpeed_ * frameDeltaTime_->GetDeltaTime()) * quaternion;

  swingTimer_ -= frameDeltaTime_->GetDeltaTime();
  if (swingTimer_ <= 0.0f) {
    swingTimer_ = 0.0f;
    pivot_->RefTransform() = pivot_->GetInitialTransform();
  }
}

void CmpSwing::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpSwing::StartSwing() {
  if (!pivot_) {
    return;
  }

  pivot_->RefTransform() = pivot_->GetInitialTransform();
  swingTimer_ = swingDuration_;
}

} // namespace Atrum
