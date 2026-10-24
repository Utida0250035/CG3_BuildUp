#include "System/CmpOrbiter.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace Atrum {

void CmpOrbiter::ResolveDependence() { frameDeltaTime_ = FrameDeltaTime::Instance(); }

void CmpOrbiter::Update() {
  const float deltaTime = frameDeltaTime_->GetDeltaTime();

  if (isMoveToSweepStartActive_ || isGatherToDefaultActive_ ||
      isGatherTrajectoryHoldActive_) {
    return;
  }

  if (isSpinUnderBossActive_) {
    sweepAngleRadians_ += sweepRotateSpeed_ * deltaTime;
    const float c = std::cos(sweepAngleRadians_);
    const float s = std::sin(sweepAngleRadians_);
    RefOwner().RefTransform().translate = {c * spinHubRadius_, sweepLocalY_,
                                           s * spinHubRadius_};
    return;
  }

  if (isGroundSweepActive_) {
    sweepAngleRadians_ += sweepRotateSpeed_ * deltaTime;
    const float radius =
        sweepHubRadius_ +
        (sweepMaxRadius_ - sweepHubRadius_) * sweepRadiusScale_;
    const float c = std::cos(sweepAngleRadians_);
    const float s = std::sin(sweepAngleRadians_);
    RefOwner().RefTransform().translate = {c * radius, sweepLocalY_, s * radius};
    return;
  }

  const Math::Quaternion deltaQ =
      Math::Quaternion::FromAxisAngle(rotateAxis_, rotateSpeed_ * deltaTime);
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
  EndMoveToSweepStart();
  EndGatherToDefaultPosition();
  EndGatherTrajectoryHold();
  EndSpinUnderBoss();
  EndGroundSweep();

  if (!hasDefaults_) {
    return;
  }

  rotateAxis_ = defaultAxis_;
  rotateSpeed_ = defaultSpeed_;
  RefOwner().RefTransform().translate = defaultTranslate_;
}

void CmpOrbiter::BeginMoveToSweepStart(const float localOrbitHeightY,
                                       const float hubRadius,
                                       const float startAngleRadians) {
  isMoveToSweepStartActive_ = true;
  moveStartTranslate_ = RefOwner().GetTransform().translate;
  const float c = std::cos(startAngleRadians);
  const float s = std::sin(startAngleRadians);
  moveTargetTranslate_ = {c * hubRadius, localOrbitHeightY, s * hubRadius};
}

void CmpOrbiter::SetMoveToSweepStartProgress(const float progress01) {
  if (!isMoveToSweepStartActive_) {
    return;
  }

  const float t = std::clamp(progress01, 0.0f, 1.0f);
  if (t <= 0.0f) {
    RefOwner().RefTransform().translate = moveStartTranslate_;
    return;
  }
  if (t >= 1.0f) {
    RefOwner().RefTransform().translate = moveTargetTranslate_;
    return;
  }

  Math::Vector3 from = moveStartTranslate_;
  const float fromLength = from.Length();
  const float toLength = moveTargetTranslate_.Length();

  if (fromLength < 1.0e-5f) {
    RefOwner().RefTransform().translate =
        Math::Lerp(moveStartTranslate_, moveTargetTranslate_, t);
    return;
  }

  from /= fromLength;

  Math::Vector3 toDirection = Math::Vector3::UpLh();
  if (toLength >= 1.0e-5f) {
    toDirection = moveTargetTranslate_ / toLength;
  } else if (moveTargetTranslate_.y < 0.0f) {
    toDirection = -Math::Vector3::UpLh();
  }

  const Math::Quaternion rotationFromStartToTarget =
      Math::Quaternion::FromLhTwoDirection(from, toDirection);
  const Math::Quaternion partialRotation = Math::Quaternion::Slerp(
      Math::Quaternion::Identity(), rotationFromStartToTarget, t);

  const Math::Vector3 direction = partialRotation.RotateVector(from);
  const float radius = Math::Lerp(fromLength, toLength, t);
  RefOwner().RefTransform().translate = direction * radius;
}

void CmpOrbiter::EndMoveToSweepStart() {
  if (!isMoveToSweepStartActive_) {
    return;
  }

  isMoveToSweepStartActive_ = false;
  RefOwner().RefTransform().translate = moveTargetTranslate_;
}

void CmpOrbiter::BeginGatherToDefaultPosition() {
  if (!hasDefaults_) {
    return;
  }

  EndSpinUnderBoss();
  EndGroundSweep();
  EndMoveToSweepStart();

  isGatherToDefaultActive_ = true;
  moveStartTranslate_ = RefOwner().GetTransform().translate;
  moveTargetTranslate_ = defaultTranslate_;
}

void CmpOrbiter::SetGatherToDefaultProgress(const float progress01) {
  if (!isGatherToDefaultActive_) {
    return;
  }

  const float t = std::clamp(progress01, 0.0f, 1.0f);
  RefOwner().RefTransform().translate =
      Math::Lerp(moveStartTranslate_, moveTargetTranslate_, t);
}

void CmpOrbiter::EndGatherToDefaultPosition() {
  if (!isGatherToDefaultActive_) {
    return;
  }

  isGatherToDefaultActive_ = false;
  RefOwner().RefTransform().translate = moveTargetTranslate_;
  rotateAxis_ = defaultAxis_;
  rotateSpeed_ = defaultSpeed_;
}

Math::Vector3 CmpOrbiter::GetGatherTargetWorldPosition() {
  if (!hasDefaults_) {
    return RefOwner().GetWorldPosition();
  }

  const Entity *parent = RefOwner().GetParent();
  if (parent == nullptr) {
    return defaultTranslate_;
  }

  Math::TransformLH localTransform{};
  localTransform.translate = defaultTranslate_;
  const Math::Matrix4x4 worldMatrix =
      localTransform.MakeWorldMatrix() * parent->GetWorldMatrix();

  return {worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]};
}

void CmpOrbiter::BeginSpinUnderBoss(const float rotateSpeed,
                                    const float localOrbitHeightY,
                                    const float startAngleRadians,
                                    const float hubRadius) {
  isSpinUnderBossActive_ = true;
  isGroundSweepActive_ = false;
  spinHubRadius_ = hubRadius;
  sweepLocalY_ = localOrbitHeightY;
  sweepAngleRadians_ = startAngleRadians;
  sweepRotateSpeed_ = rotateSpeed;
}

void CmpOrbiter::EndSpinUnderBoss() { isSpinUnderBossActive_ = false; }

void CmpOrbiter::BeginGroundSweep(const float rotateSpeed,
                                  const float maxOrbitRadius,
                                  const float localOrbitHeightY,
                                  const float startAngleRadians,
                                  const float hubRadius) {
  isSpinUnderBossActive_ = false;
  isGroundSweepActive_ = true;
  sweepMaxRadius_ = maxOrbitRadius;
  sweepHubRadius_ = hubRadius;
  sweepLocalY_ = localOrbitHeightY;
  sweepAngleRadians_ = startAngleRadians;
  sweepRotateSpeed_ = rotateSpeed;
  sweepRadiusScale_ = 0.0f;
  const float c = std::cos(startAngleRadians);
  const float s = std::sin(startAngleRadians);
  RefOwner().RefTransform().translate = {c * hubRadius, localOrbitHeightY,
                                         s * hubRadius};
}

void CmpOrbiter::BeginGroundSweepExpandFromSpin(
    const float expandRotateSpeed, const float maxOrbitRadius) {
  isSpinUnderBossActive_ = false;
  isGroundSweepActive_ = true;
  sweepMaxRadius_ = maxOrbitRadius;
  sweepHubRadius_ = spinHubRadius_;
  sweepRotateSpeed_ = expandRotateSpeed;
  sweepRadiusScale_ = 0.0f;
}

void CmpOrbiter::SetGroundSweepRadiusScale(const float scale01) {
  if (!isGroundSweepActive_) {
    return;
  }

  if (scale01 <= 0.0f) {
    sweepRadiusScale_ = 0.0f;
  } else if (scale01 >= 1.0f) {
    sweepRadiusScale_ = 1.0f;
  } else {
    sweepRadiusScale_ = scale01;
  }
}

void CmpOrbiter::EndGroundSweep() { isGroundSweepActive_ = false; }

void CmpOrbiter::BeginGatherTrajectoryHold() {
  EndSpinUnderBoss();
  EndGroundSweep();
  EndMoveToSweepStart();
  isGatherTrajectoryHoldActive_ = true;
}

void CmpOrbiter::EndGatherTrajectoryHold() {
  isGatherTrajectoryHoldActive_ = false;
}

} // namespace Atrum
