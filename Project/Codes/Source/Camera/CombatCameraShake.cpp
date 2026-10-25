#include "Camera/CombatCameraShake.h"

#include "Camera/Camera.h"
#include "Camera/StageViewCamera.h"
#include "Math/Vector3.h"

#include <cmath>

namespace Atrum {

CombatCameraShake &CombatCameraShake::Instance() {
  static CombatCameraShake instance;
  return instance;
}

void CombatCameraShake::Reset() {
  timer_ = 0.0f;
  duration_ = 0.0f;
  amplitude_ = 0.0f;
}

void CombatCameraShake::Trigger(const float amplitude,
                               const float durationSeconds) {
  amplitude_ = amplitude;
  duration_ = durationSeconds;
  timer_ = durationSeconds;
}

void CombatCameraShake::RequestBossHitShake() {
  Trigger(0.14f, 0.32f);
}

void CombatCameraShake::RequestPlayerHitShake() {
  Trigger(0.38f, 0.42f);
}

void CombatCameraShake::Update(const float deltaTimeSeconds) {
  if (timer_ <= 0.0f) {
    return;
  }

  timer_ -= deltaTimeSeconds;
  if (timer_ < 0.0f) {
    timer_ = 0.0f;
  }
}

Math::Vector3 CombatCameraShake::SampleOffset() const {
  if (timer_ <= 0.0f || duration_ <= 0.0f) {
    return {};
  }

  const float life01 = timer_ / duration_;
  const float strength = amplitude_ * life01;
  const float elapsed = duration_ - timer_;
  const float wave = elapsed * 48.0f;
  return {std::sin(wave * 1.3f) * strength, std::sin(wave * 1.9f) * strength,
          std::sin(wave * 0.7f) * strength * 0.35f};
}

void CombatCameraShake::ApplyStageBattleView(Camera *camera) {
  if (camera == nullptr) {
    return;
  }

  StageViewCameraPose pose = ComputeStageBattleViewPose();
  if (timer_ > 0.0f) {
    const Math::Vector3 offset = SampleOffset();
    pose.eye += offset;
    pose.pivot += offset * 0.35f;
  }

  ApplyStageViewCameraPose(camera, pose);
}

} // namespace Atrum
