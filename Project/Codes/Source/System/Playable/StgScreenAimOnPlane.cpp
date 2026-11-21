#include "System/Playable/StgScreenAimOnPlane.h"

#include "Camera/Camera.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector4.h"

#include <cmath>

namespace Atrum::Player {

namespace {

Math::Vector4 MultiplyHomogeneous(const Math::Matrix4x4 &m, const float x,
                                    const float y, const float z,
                                    const float w) {
  return {
      m.m[0][0] * x + m.m[0][1] * y + m.m[0][2] * z + m.m[0][3] * w,
      m.m[1][0] * x + m.m[1][1] * y + m.m[1][2] * z + m.m[1][3] * w,
      m.m[2][0] * x + m.m[2][1] * y + m.m[2][2] * z + m.m[2][3] * w,
      m.m[3][0] * x + m.m[3][1] * y + m.m[3][2] * z + m.m[3][3] * w,
  };
}

Math::Vector3 HomogeneousToPoint(const Math::Vector4 &h) {
  if (std::abs(h.w) < 1e-6f) {
    return {};
  }
  const float invW = 1.0f / h.w;
  return {h.x * invW, h.y * invW, h.z * invW};
}

} // namespace

bool TryScreenPointToHorizontalPlaneY(const Camera &camera, const float screenX,
                                    const float screenY, const float viewWidth,
                                    const float viewHeight, const float planeY,
                                    Math::Vector3 &outWorldPoint) {
  if (viewWidth < 1.0f || viewHeight < 1.0f) {
    return false;
  }

  const float ndcX = (screenX / viewWidth) * 2.0f - 1.0f;
  const float ndcY = 1.0f - (screenY / viewHeight) * 2.0f;

  const Math::Matrix4x4 invVp = camera.GetViewProjectionMatrix().Inversed();
  const Math::Vector3 nearPt =
      HomogeneousToPoint(MultiplyHomogeneous(invVp, ndcX, ndcY, 0.0f, 1.0f));
  const Math::Vector3 farPt =
      HomogeneousToPoint(MultiplyHomogeneous(invVp, ndcX, ndcY, 1.0f, 1.0f));
  const Math::Vector3 rayDir = (farPt - nearPt).Normalized();
  if (std::abs(rayDir.y) < 1e-5f) {
    return false;
  }
  const float t = (planeY - nearPt.y) / rayDir.y;
  if (t < 0.0f) {
    return false;
  }
  outWorldPoint = nearPt + rayDir * t;
  return true;
}

} // namespace Atrum::Player