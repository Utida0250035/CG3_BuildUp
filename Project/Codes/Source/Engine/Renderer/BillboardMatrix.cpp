#include "Engine/Renderer/BillboardMatrix.h"

#include <algorithm>

namespace Atrum {

Math::Matrix4x4 MakeViewPlaneParallelBillboardWorld(
    const Math::Vector3 &worldPosition, const Math::Vector3 &worldScale,
    const Math::Matrix4x4 &viewMatrix) {

  const Math::Matrix4x4 cameraWorld = viewMatrix.Inversed();

  Math::Matrix4x4 rotation = Math::Matrix4x4::Identity();
  for (size_t row = 0; row < 3; ++row) {
    for (size_t col = 0; col < 3; ++col) {
      rotation.m[row][col] = cameraWorld.m[row][col];
    }
  }

  const Math::Vector3 halfScale{
      std::max(worldScale.x, 1e-5f) * 0.5f,
      std::max(worldScale.y, 1e-5f) * 0.5f,
      1.0f,
  };

  return Math::Matrix4x4::Scale(halfScale) * rotation *
         Math::Matrix4x4::Translate(worldPosition);
}

} // namespace Atrum
