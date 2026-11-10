#include "Camera/CameraController/MovieCameraController.h"

#include "Camera/MovieCameraTrackLoader.h"
#include "System/DeltaTime.h"

#include <algorithm>

namespace Atrum {

void MovieCameraController::Begin(std::vector<MovieCameraKeyframe> keyframes) {
  if (keyframes.size() < 2) {
    isPlaying_ = false;
    keyframes_.clear();
    return;
  }

  std::sort(keyframes.begin(), keyframes.end(),
            [](const MovieCameraKeyframe &a, const MovieCameraKeyframe &b) {
              return a.timeSeconds < b.timeSeconds;
            });

  keyframes_ = std::move(keyframes);
  totalDurationSeconds_ = keyframes_.back().timeSeconds;
  playbackTimeSeconds_ = 0.0f;
  isPlaying_ = totalDurationSeconds_ > 0.0f;

  ApplyPoseAtTime(0.0f);
}

bool MovieCameraController::TryBeginFromJsonFile(const std::string &jsonPath) {
  std::vector<MovieCameraKeyframe> loaded{};
  if (!LoadMovieCameraTrackFromJson(jsonPath, loaded)) {
    return false;
  }

  Begin(std::move(loaded));
  return isPlaying_;
}

void MovieCameraController::Update() {
  if (!isPlaying_ || camera_ == nullptr) {
    return;
  }

  playbackTimeSeconds_ += ::Atrum::EngineFrameDeltaTime()->GetDeltaTime();
  ApplyPoseAtTime(playbackTimeSeconds_);

  if (playbackTimeSeconds_ >= totalDurationSeconds_) {
    isPlaying_ = false;
  }
}

bool MovieCameraController::IsFinished() const {
  return !keyframes_.empty() && playbackTimeSeconds_ >= totalDurationSeconds_;
}

void MovieCameraController::ApplyPoseAtTime(const float timeSeconds) const {
  if (camera_ == nullptr || keyframes_.empty()) {
    return;
  }

  const float clamped = std::clamp(timeSeconds, 0.0f, totalDurationSeconds_);

  if (keyframes_.size() == 1 || clamped >= keyframes_.back().timeSeconds) {
    ApplyStageViewPose(keyframes_.back().pose);
    return;
  }

  size_t nextIndex = 1;
  while (nextIndex < keyframes_.size() &&
         keyframes_[nextIndex].timeSeconds <= clamped) {
    ++nextIndex;
  }

  const size_t prevIndex = nextIndex - 1;
  const MovieCameraKeyframe &fromKf = keyframes_[prevIndex];
  const MovieCameraKeyframe &toKf = keyframes_[nextIndex];

  const float segmentDuration =
      std::max(toKf.timeSeconds - fromKf.timeSeconds, 1.0e-4f);
  const float segmentT =
      std::clamp((clamped - fromKf.timeSeconds) / segmentDuration, 0.0f, 1.0f);
  const float eased =
      EvaluateStageViewCameraEase(fromKf.easeToNext, segmentT);

  ApplyStageViewPose(
      LerpStageViewCameraPose(fromKf.pose, toKf.pose, eased));
}

} // namespace Atrum
