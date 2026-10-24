#include "Camera/CameraController/CameraGameIntroController.h"

#include "System/DeltaTime.h"

#include <algorithm>

namespace Atrum {

void CameraGameIntroController::Initialize() {
  pullBackBlend_.Initialize();
}

void CameraGameIntroController::BeginIntro(
    const Math::Vector3 &bossWorldPosition) {
  phase_ = Phase::HpBarReveal;
  phaseElapsedSeconds_ = 0.0f;

  stageBattlePose_ = ComputeStageBattleViewPose();
  bossFocusPose_ = ComputeFocusViewPose(
      bossWorldPosition, kBossIntroFocusSubject.halfHeight,
      kBossIntroFocusSubject.halfWidth);

  ApplyStageViewPose(bossFocusPose_);
}

void CameraGameIntroController::Update() {
  if (camera_ == nullptr || phase_ == Phase::Finished) {
    return;
  }

  const float deltaTime = FrameDeltaTime::Instance()->GetDeltaTime();
  phaseElapsedSeconds_ += deltaTime;

  if (phase_ == Phase::HpBarReveal) {
    ApplyStageViewPose(bossFocusPose_);
    if (phaseElapsedSeconds_ >= kHpBarRevealDurationSeconds) {
      phase_ = Phase::CameraPullBack;
      phaseElapsedSeconds_ = 0.0f;
      pullBackBlend_.SetCamera(camera_);
      pullBackBlend_.Begin(bossFocusPose_, stageBattlePose_,
                           kCameraPullBackDurationSeconds);
    }
    return;
  }

  if (phase_ == Phase::CameraPullBack) {
    pullBackBlend_.Update();
    if (pullBackBlend_.IsFinished()) {
      FinishIntro();
    }
  }
}

void CameraGameIntroController::SkipIntro() {
  if (phase_ == Phase::Finished) {
    return;
  }

  FinishIntro();
}

void CameraGameIntroController::FinishIntro() {
  phase_ = Phase::Finished;
  phaseElapsedSeconds_ = 0.0f;
  ApplyStageViewPose(stageBattlePose_);
  SyncDebugCameraFromStageViewPose(stageBattlePose_);
}

float CameraGameIntroController::GetBossHpBarReveal01() const {
  if (phase_ == Phase::Finished || phase_ == Phase::CameraPullBack) {
    return 1.0f;
  }

  return std::clamp(phaseElapsedSeconds_ / kHpBarRevealDurationSeconds, 0.0f,
                    1.0f);
}

bool CameraGameIntroController::IsFinished() const {
  return phase_ == Phase::Finished;
}

} // namespace Atrum
