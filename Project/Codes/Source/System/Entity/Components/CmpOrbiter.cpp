#include "System/Entity/Components/CmpOrbiter.h"

#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

#include <cmath>

namespace Atrum {

namespace {

float OrbitRadiusFromTranslate(const Math::Vector3 &translate) {
  return std::hypot(translate.x, translate.y);
}

} // namespace

void CmpOrbiter::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpOrbiter::SetRotateSpeed(const float rotateSpeed) {
  rotateSpeed_ = rotateSpeed;
  if (baselineRotateSpeed_ == 0.0f) {
    baselineRotateSpeed_ = rotateSpeed;
  }
}

void CmpOrbiter::ApplyRotateSpeedScale(const float scale) {
  if (baselineRotateSpeed_ == 0.0f) {
    baselineRotateSpeed_ = rotateSpeed_;
  }

  rotateSpeed_ = baselineRotateSpeed_ * scale;
  if (hasDefaults_) {
    defaultSpeed_ = rotateSpeed_;
  }
}

void CmpOrbiter::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }

  const float deltaTime = frameDeltaTime_->GetDeltaTime();
  if (!std::isfinite(deltaTime) || deltaTime <= 0.0f) {
    return;
  }

  float angularSpeed = rotateSpeed_;
  if (!std::isfinite(angularSpeed)) {
    angularSpeed = 0.0f;
  }

  Math::Vector3 &translate = RefOwner().RefTransform().translate;
  translate.z = 0.0f;

  float radius = hasDefaults_ ? OrbitRadiusFromTranslate(defaultTranslate_)
                              : OrbitRadiusFromTranslate(translate);
  if (radius <= 1.0e-6f) {
    radius = 1.0f;
    translate = {radius, 0.0f, 0.0f};
    return;
  }

  float angle = std::atan2(translate.y, translate.x);
  if (!std::isfinite(angle)) {
    angle = 0.0f;
  }

  angle += angularSpeed * deltaTime;
  translate.x = std::cos(angle) * radius;
  translate.y = std::sin(angle) * radius;
  translate.z = 0.0f;
}

void CmpOrbiter::CaptureDefaults() {
  defaultAxis_ = rotateAxis_;
  defaultSpeed_ = rotateSpeed_;
  defaultTranslate_ = RefOwner().GetTransform().translate;
  defaultTranslate_.z = 0.0f;
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
