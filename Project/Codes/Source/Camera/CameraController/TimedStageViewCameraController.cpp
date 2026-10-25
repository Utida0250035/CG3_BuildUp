#include "Camera/CameraController/TimedStageViewCameraController.h"

#include "System/DeltaTime.h"

#include <algorithm>

namespace Atrum {

void TimedStageViewCameraController::BeginTiming(const float durationSeconds) {
  isActive_ = true;
  elapsedSeconds_ = 0.0f;
  durationSeconds_ = std::max(durationSeconds, 1.0e-4f);
}

void TimedStageViewCameraController::TickTiming() {
  if (!isActive_) {
    return;
  }

  elapsedSeconds_ += FrameDeltaTime::Instance()->GetDeltaTime();
}

float TimedStageViewCameraController::GetTimedProgress01() const {
  if (!isActive_) {
    return 0.0f;
  }

  return std::clamp(elapsedSeconds_ / durationSeconds_, 0.0f, 1.0f);
}

} // namespace Atrum
