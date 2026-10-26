#include "Engine/Renderer/Line3DGeometry.h"

#include <algorithm>
#include <cmath>

namespace Atrum::Renderer {

bool BuildCameraFacingLineQuad(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const float width, const Math::Vector3 &cameraWorldPosition,
    std::array<Math::Vector3, 4> &outPositions, Math::Vector3 &outNormal) {

  const Math::Vector3 delta = worldEnd - worldStart;
  const float length = delta.Length();
  const float clampedWidth = std::max(width, 1.0e-5f);
  if (length < 1.0e-4f) {
    return false;
  }

  Math::Vector3 lineDir = delta / length;
  const Math::Vector3 center = (worldStart + worldEnd) * 0.5f;

  Math::Vector3 toCamera = cameraWorldPosition - center;
  if (toCamera.LengthSquare() < 1.0e-8f) {
    toCamera = Math::Vector3{0.0f, 0.0f, 1.0f};
  } else {
    toCamera.Normalize();
  }

  Math::Vector3 side = lineDir.CrossLh(toCamera);
  if (side.LengthSquare() < 1.0e-8f) {
    side = lineDir.CrossLh(Math::Vector3::UpLh());
  }
  side.Normalize();
  outNormal = side;

  const float halfLength = length * 0.5f;
  const float halfWidth = clampedWidth * 0.5f;

  const Math::Vector3 alongOffset = lineDir * halfLength;
  const Math::Vector3 widthOffset = side * halfWidth;

  outPositions[0] = center - alongOffset - widthOffset;
  outPositions[1] = center + alongOffset - widthOffset;
  outPositions[2] = center - alongOffset + widthOffset;
  outPositions[3] = center + alongOffset + widthOffset;

  return true;
}

} // namespace Atrum::Renderer
