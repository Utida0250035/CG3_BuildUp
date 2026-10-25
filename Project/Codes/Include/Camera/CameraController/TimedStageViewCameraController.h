#pragma once

#include "Camera/CameraController/CameraController.h"

namespace Atrum {

/// 時間進行付きステージビューカメラ演出の共通基盤
class TimedStageViewCameraController : public CameraController {
protected:
  bool isActive_ = false;
  float elapsedSeconds_ = 0.0f;
  float durationSeconds_ = 1.0f;

  void BeginTiming(float durationSeconds);
  void TickTiming();
  float GetTimedProgress01() const;
};

} // namespace Atrum
