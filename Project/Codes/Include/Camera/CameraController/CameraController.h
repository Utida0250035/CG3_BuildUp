#pragma once

#include "Camera/StageViewCamera.h"

namespace Atrum {

class Camera;

class CameraController {
protected:
  Camera *camera_ = nullptr;

  void ApplyStageViewPose(const StageViewCameraPose &pose) const;
  void SyncDebugCameraFromStageViewPose(const StageViewCameraPose &pose) const;

public:
  virtual void Initialize() = 0;
  virtual void Update() = 0;
  virtual void Finalize() {}

  void SetCamera(Camera *camera) { camera_ = camera; }

  CameraController() = default;
  virtual ~CameraController() = default;
};

} // namespace Atrum