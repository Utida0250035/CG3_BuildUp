#include "Camera/CameraController/CameraPoseBlendController.h"

#include "Math/Easing.h"

namespace Atrum {

void CameraPoseBlendController::Begin(const StageViewCameraPose &fromPose,
                                      const StageViewCameraPose &toPose,
                                      const float durationSeconds) {
  fromPose_ = fromPose;
  toPose_ = toPose;
  BeginTiming(durationSeconds);
  ApplyBlendedPose();
}

void CameraPoseBlendController::Update() {
  if (!isActive_ || camera_ == nullptr) {
    return;
  }

  TickTiming();
  ApplyBlendedPose();
}

bool CameraPoseBlendController::IsFinished() const {
  return isActive_ && GetTimedProgress01() >= 1.0f;
}

void CameraPoseBlendController::ApplyBlendedPose() const {
  if (camera_ == nullptr) {
    return;
  }

  const float blendProgress =
      Math::Ease::InOutQuad(GetTimedProgress01());
  ApplyStageViewPose(
      LerpStageViewCameraPose(fromPose_, toPose_, blendProgress));
}

} // namespace Atrum
