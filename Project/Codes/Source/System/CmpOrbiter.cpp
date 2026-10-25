#include "System/CmpOrbiter.h"

#include "Math/Quaternion.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void CmpOrbiter::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpOrbiter::SetRotateSpeed(const float rotateSpeed) {
  rotateSpeed_ = rotateSpeed;
  if (baselineRotateSpeed_ <= 0.0f) {
    baselineRotateSpeed_ = rotateSpeed;
  }
}

void CmpOrbiter::ApplyRotateSpeedScale(const float scale) {
  if (baselineRotateSpeed_ <= 0.0f) {
    baselineRotateSpeed_ = rotateSpeed_;
  }

  rotateSpeed_ = baselineRotateSpeed_ * scale;
  if (hasDefaults_) {
    defaultSpeed_ = rotateSpeed_;
  }
}

void CmpOrbiter::Update() {
  const Math::Quaternion deltaQ = Math::Quaternion::FromAxisAngle(
      rotateAxis_, rotateSpeed_ * frameDeltaTime_->GetDeltaTime());
  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate = deltaQ.RotateVector(translate);
}

void CmpOrbiter::CaptureDefaults() {
  defaultAxis_ = rotateAxis_;
  defaultSpeed_ = rotateSpeed_;
  defaultTranslate_ = RefOwner().GetTransform().translate;
  hasDefaults_ = true;
}

void CmpOrbiter::RestoreDefaults() {
  if (!hasDefaults_) {
    return;
  }

  rotateAxis_ = defaultAxis_;
  rotateSpeed_ = defaultSpeed_;
  RefOwner().RefTransform().translate = defaultTranslate_;
}

} // namespace Atrum
