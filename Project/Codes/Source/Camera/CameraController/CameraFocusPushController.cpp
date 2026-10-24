#include "Camera/CameraController/CameraFocusPushController.h"

#include "Math/Easing.h"

namespace Atrum {

void CameraFocusPushController::Begin(const StageViewCameraPose &startPose,
                                      const float pushDurationSeconds,
                                      const float subjectHalfHeight,
                                      const float subjectHalfWidth) {
  startPose_ = startPose;
  subjectHalfHeight_ = subjectHalfHeight;
  subjectHalfWidth_ = subjectHalfWidth;
  BeginTiming(pushDurationSeconds);
  ApplyCurrentPose();
}

void CameraFocusPushController::BeginFromStageBattleView(
    const float pushDurationSeconds, const float subjectHalfHeight,
    const float subjectHalfWidth) {
  Begin(ComputeStageBattleViewPose(), pushDurationSeconds, subjectHalfHeight,
        subjectHalfWidth);
}

void CameraFocusPushController::BeginBossDefeatFromStageBattle(
    Camera *camera, const Math::Vector3 &initialFocusWorld) {
  SetCamera(camera);
  BeginFromStageBattleView(kBossDefeatCameraPushSeconds,
                           kBossDefeatFocusSubject.halfHeight,
                           kBossDefeatFocusSubject.halfWidth);
  SetFocusWorldPosition(initialFocusWorld);
}

void CameraFocusPushController::SetFocusWorldPosition(
    const Math::Vector3 &focusWorld) {
  focusWorld_ = focusWorld;
}

void CameraFocusPushController::Update() {
  if (!isActive_ || camera_ == nullptr) {
    return;
  }

  TickTiming();
  ApplyCurrentPose();
}

float CameraFocusPushController::GetPushProgress01() const {
  return GetTimedProgress01();
}

bool CameraFocusPushController::IsPushFinished() const {
  return isActive_ && GetTimedProgress01() >= 1.0f;
}

void CameraFocusPushController::ApplyCurrentPose() const {
  if (camera_ == nullptr) {
    return;
  }

  const float pushProgress = Math::Ease::InOutQuad(GetTimedProgress01());
  const StageViewCameraPose focusPose =
      ComputeFocusViewPose(focusWorld_, subjectHalfHeight_, subjectHalfWidth_);

  ApplyStageViewPose(
      LerpStageViewCameraPose(startPose_, focusPose, pushProgress));
}

} // namespace Atrum
