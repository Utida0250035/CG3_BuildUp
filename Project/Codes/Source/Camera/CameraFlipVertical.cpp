#include "Camera/CameraFlipVertical.h"

#include <numbers>

namespace Atrum {

void FlipCameraVertical(Camera &camera) {
  const Math::Quaternion flip = Math::Quaternion::FromAxisAngle(
      Math::Vector3::RightLh(), std::numbers::pi_v<float>);
  camera.AddLocalRotation(flip);
}

} // namespace Atrum
