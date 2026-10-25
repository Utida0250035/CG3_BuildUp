#include "Camera/CameraController/CameraController.h"

#include "Camera/StageViewCamera.h"

#if defined(DEVELOPMENT) && !defined(NDEBUG)
#include "ForDebug/DebugCamera.h"
#endif

#include <cmath>

namespace Atrum {

void CameraController::ApplyStageViewPose(
    const StageViewCameraPose &pose) const {
  ApplyStageViewCameraPose(camera_, pose);
}

void CameraController::SyncDebugCameraFromStageViewPose(
    const StageViewCameraPose &pose) const {
#if defined(DEVELOPMENT) && !defined(NDEBUG)
  if (camera_ == nullptr) {
    return;
  }

  if (auto *debugCam = dynamic_cast<Debug::DebugCamera *>(camera_)) {
    const Math::Vector3 offset = pose.eye - pose.pivot;
    debugCam->RefPivot() = pose.pivot;
    debugCam->RefDistance() = std::abs(offset.z);
  }
#else
  (void)pose;
#endif
}

} // namespace Atrum
