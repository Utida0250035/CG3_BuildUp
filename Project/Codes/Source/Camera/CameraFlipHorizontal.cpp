#include "Camera/CameraFlipHorizontal.h"

#include <numbers>

namespace Atrum {

void FlipCameraHorizontal(Camera &camera) {
  const Math::Quaternion flip = Math::Quaternion::FromAxisAngle(
      Math::Vector3::UpLh(), std::numbers::pi_v<float>);
  camera.AddLocalRotation(flip);
}

} // namespace Atrum
