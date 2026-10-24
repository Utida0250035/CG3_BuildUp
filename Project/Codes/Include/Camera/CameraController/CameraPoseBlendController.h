#pragma once

#include "Camera/CameraController/TimedStageViewCameraController.h"
#include "Camera/StageViewCamera.h"

namespace Atrum {

/// 2 つの StageView カメラ姿勢を時間補間する汎用演出
class CameraPoseBlendController final : public TimedStageViewCameraController {
public:
  void Initialize() override {}

  void Begin(const StageViewCameraPose &fromPose,
             const StageViewCameraPose &toPose, float durationSeconds);

  void Update() override;

  bool IsFinished() const;

private:
  StageViewCameraPose fromPose_{};
  StageViewCameraPose toPose_{};

  void ApplyBlendedPose() const;
};

} // namespace Atrum
