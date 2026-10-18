#pragma once

#include "Camera/Camera.h"

namespace Atrum {

class CameraController {
protected:
  Camera *camera_ = nullptr;

public:
  virtual void Initialize() = 0;
  virtual void Update() = 0;
  virtual void Finalize() {}

  void SetCamera(Camera *camera) { camera_ = camera; }

  CameraController() = default;
  virtual ~CameraController() = default;
};

} // namespace Atrum