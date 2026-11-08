#pragma once

#include "Camera/CameraController/CameraController.h"
#include "Camera/StageViewCamera.h"
#include "Camera/StageViewCameraEasing.h"
#include "Math/Vector3.h"

#include <string>
#include <vector>

namespace Atrum {

struct MovieCameraKeyframe {
  float timeSeconds = 0.0f;
  StageViewCameraPose pose{};
  StageViewCameraEaseKind easeToNext = StageViewCameraEaseKind::PullBackReveal;
};

/// キーフレーム列で StageView カメラを再生（ムービー／カットシーン用）
class MovieCameraController final : public CameraController {
public:
  void Initialize() override {}

  /// 2 点以上・timeSeconds 昇順を推奨（Begin 内でソート）
  void Begin(std::vector<MovieCameraKeyframe> keyframes);

  bool TryBeginFromJsonFile(const std::string &jsonPath);

  void Update() override;

  bool IsFinished() const;

  float GetPlaybackTimeSeconds() const { return playbackTimeSeconds_; }

  float GetTotalDurationSeconds() const { return totalDurationSeconds_; }

private:
  std::vector<MovieCameraKeyframe> keyframes_{};
  float playbackTimeSeconds_ = 0.0f;
  float totalDurationSeconds_ = 0.0f;
  bool isPlaying_ = false;

  void ApplyPoseAtTime(const float timeSeconds) const;
};

} // namespace Atrum
