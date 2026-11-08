#include "Engine/Renderer/Line3DGeometry.h"

#include "Math/Vector2.h"

#include <algorithm>
#include <cmath>

namespace Atrum::Renderer {

namespace {

bool TryProjectWorldToNdc(const Math::Matrix4x4 &matrix,
                          const Math::Vector3 &world, Math::Vector2 &outNdc) {
  const float clipX = world.x * matrix.m[0][0] + world.y * matrix.m[1][0] +
                      world.z * matrix.m[2][0] + matrix.m[3][0];
  const float clipY = world.x * matrix.m[0][1] + world.y * matrix.m[1][1] +
                      world.z * matrix.m[2][1] + matrix.m[3][1];
  const float clipW = world.x * matrix.m[0][3] + world.y * matrix.m[1][3] +
                      world.z * matrix.m[2][3] + matrix.m[3][3];
  if (clipW <= 1.0e-5f) {
    return false;
  }
  const float invW = 1.0f / clipW;
  outNdc.x = clipX * invW;
  outNdc.y = clipY * invW;
  return std::isfinite(outNdc.x) && std::isfinite(outNdc.y);
}

bool IntersectRayWithPlaneY(const Math::Vector3 &rayOrigin,
                            const Math::Vector3 &rayDirection,
                            const float planeY, Math::Vector3 &hitPoint) {
  if (std::abs(rayDirection.y) < 1.0e-6f) {
    return false;
  }
  const float t = (planeY - rayOrigin.y) / rayDirection.y;
  if (t < 0.0f) {
    return false;
  }
  hitPoint = rayOrigin + rayDirection * t;
  return true;
}

bool WorldPointOnFloorFromNdc(
    const Math::Vector2 &ndc, const Math::Vector3 &cameraWorldPosition,
    const Math::Matrix4x4 &viewMatrix,
    const Math::Matrix4x4 &perspectiveMatrix, const float floorY,
    Math::Vector3 &outWorld) {
  Math::Vector3 forward{viewMatrix.m[2][0], viewMatrix.m[2][1],
                        viewMatrix.m[2][2]};
  Math::Vector3 right{viewMatrix.m[0][0], viewMatrix.m[0][1], viewMatrix.m[0][2]};
  Math::Vector3 up{viewMatrix.m[1][0], viewMatrix.m[1][1], viewMatrix.m[1][2]};
  if (forward.LengthSquare() < 1.0e-10f) {
    return false;
  }
  forward.Normalize();
  right.Normalize();
  up.Normalize();

  const float cot = perspectiveMatrix.m[1][1];
  if (std::abs(cot) < 1.0e-6f) {
    return false;
  }
  const float tanHalfFovY = 1.0f / cot;
  const float aspect =
      (std::abs(perspectiveMatrix.m[0][0]) > 1.0e-6f)
          ? (perspectiveMatrix.m[1][1] / perspectiveMatrix.m[0][0])
          : (16.0f / 9.0f);
  const float tanHalfFovX = tanHalfFovY * aspect;

  Math::Vector3 rayDirection =
      forward + right * (ndc.x * tanHalfFovX) + up * (ndc.y * tanHalfFovY);
  if (rayDirection.LengthSquare() < 1.0e-10f) {
    return false;
  }
  rayDirection.Normalize();
  if (!IntersectRayWithPlaneY(cameraWorldPosition, rayDirection, floorY,
                              outWorld)) {
    return false;
  }
  outWorld.y = floorY;
  return true;
}

} // namespace

bool ClipWorldLineToVisibleProjection(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const Math::Matrix4x4 &viewProjection, Math::Vector3 &clippedStart,
    Math::Vector3 &clippedEnd) {
  constexpr int kSampleCount = 64;
  float firstVisibleT = -1.0f;
  float lastVisibleT = -1.0f;
  for (int i = 0; i <= kSampleCount; ++i) {
    const float t =
        static_cast<float>(i) / static_cast<float>(kSampleCount);
    const Math::Vector3 point =
        worldStart + (worldEnd - worldStart) * t;
    Math::Vector2 ndc{};
    if (!TryProjectWorldToNdc(viewProjection, point, ndc)) {
      continue;
    }
    if (firstVisibleT < 0.0f) {
      firstVisibleT = t;
    }
    lastVisibleT = t;
  }
  if (firstVisibleT < 0.0f) {
    return false;
  }
  const float padding = 1.0f / static_cast<float>(kSampleCount);
  firstVisibleT = std::max(0.0f, firstVisibleT - padding);
  lastVisibleT = std::min(1.0f, lastVisibleT + padding);
  const Math::Vector3 delta = worldEnd - worldStart;
  clippedStart = worldStart + delta * firstVisibleT;
  clippedEnd = worldStart + delta * lastVisibleT;
  return (clippedEnd - clippedStart).Length() > 1.0e-4f;
}

void ForEachVisibleLineSubsegment(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const Math::Matrix4x4 &viewProjection,
    const std::function<void(const Math::Vector3 &, const Math::Vector3 &)>
        &emitSegment) {
  constexpr int kSampleCount = 128;
  const Math::Vector3 delta = worldEnd - worldStart;

  auto isVisibleAt = [&](const float t) {
    const Math::Vector3 point = worldStart + delta * t;
    Math::Vector2 ndc{};
    return TryProjectWorldToNdc(viewProjection, point, ndc);
  };

  auto emitClipped = [&](const float t0, const float t1) {
    const float pad = 1.0f / static_cast<float>(kSampleCount);
    const float a = std::max(0.0f, t0 - pad);
    const float b = std::min(1.0f, t1 + pad);
    const Math::Vector3 segStart = worldStart + delta * a;
    const Math::Vector3 segEnd = worldStart + delta * b;
    if ((segEnd - segStart).Length() > 1.0e-4f) {
      emitSegment(segStart, segEnd);
    }
  };

  bool inVisibleRun = false;
  float runStartT = 0.0f;
  for (int i = 0; i <= kSampleCount; ++i) {
    const float t =
        static_cast<float>(i) / static_cast<float>(kSampleCount);
    const bool visible = isVisibleAt(t);
    if (visible && !inVisibleRun) {
      runStartT = t;
      inVisibleRun = true;
    } else if (!visible && inVisibleRun) {
      const float runEndT =
          static_cast<float>(i - 1) / static_cast<float>(kSampleCount);
      emitClipped(runStartT, runEndT);
      inVisibleRun = false;
    }
  }
  if (inVisibleRun) {
    emitClipped(runStartT, 1.0f);
  }
}

float EstimateMetersPerPixelAtWorldPoint(
    const Math::Vector3 &worldPoint, const Math::Vector3 &cameraWorldPosition,
    const Math::Matrix4x4 &perspectiveMatrix, const int viewportHeight) {
  const Math::Vector3 delta = worldPoint - cameraWorldPosition;
  const float viewDistance = std::max(delta.Length(), 0.25f);
  const float cot = perspectiveMatrix.m[1][1];
  if (std::abs(cot) < 1.0e-6f) {
    return 0.01f;
  }
  const float tanHalfFovY = 1.0f / cot;
  const float worldSpanY = 2.0f * viewDistance * tanHalfFovY;
  return worldSpanY / static_cast<float>(std::max(viewportHeight, 1));
}

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

bool BuildPlanarLineQuadOnFloor(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const float width, std::array<Math::Vector3, 4> &outPositions,
    Math::Vector3 &outNormal) {

  const Math::Vector3 delta = worldEnd - worldStart;
  const float length = delta.Length();
  const float clampedWidth = std::max(width, 1.0e-5f);
  if (length < 1.0e-4f) {
    return false;
  }

  const Math::Vector3 lineDir = delta / length;

  Math::Vector3 side{};
  const Math::Vector3 lineXZ{lineDir.x, 0.0f, lineDir.z};
  const float xzLen = lineXZ.Length();
  if (xzLen >= 1.0e-4f) {
    const Math::Vector3 dirXZ = lineXZ / xzLen;
    side = Math::Vector3{-dirXZ.z, 0.0f, dirXZ.x};
  } else {
    side = Math::Vector3{1.0f, 0.0f, 0.0f};
  }

  outNormal = Math::Vector3::UpLh();

  const float halfLength = length * 0.5f;
  const float halfWidth = clampedWidth * 0.5f;
  const Math::Vector3 center = (worldStart + worldEnd) * 0.5f;
  const Math::Vector3 alongOffset = lineDir * halfLength;
  const Math::Vector3 widthOffset = side * halfWidth;

  outPositions[0] = center - alongOffset - widthOffset;
  outPositions[1] = center + alongOffset - widthOffset;
  outPositions[2] = center - alongOffset + widthOffset;
  outPositions[3] = center + alongOffset + widthOffset;

  return true;
}

bool BuildScreenStableFloorLineQuad(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const float lineWidthPixels, const int viewportWidth,
    const int viewportHeight, const Math::Vector3 &cameraWorldPosition,
    const Math::Matrix4x4 &viewMatrix,
    const Math::Matrix4x4 &perspectiveMatrix, const float floorY,
    std::array<Math::Vector3, 4> &outPositions, Math::Vector3 &outNormal) {

  if (viewportWidth <= 0 || viewportHeight <= 0 || lineWidthPixels <= 0.0f) {
    return false;
  }

  const Math::Matrix4x4 viewProjection = viewMatrix * perspectiveMatrix;

  Math::Vector2 ndcStart{};
  Math::Vector2 ndcEnd{};
  if (!TryProjectWorldToNdc(viewProjection, worldStart, ndcStart) ||
      !TryProjectWorldToNdc(viewProjection, worldEnd, ndcEnd)) {
    return false;
  }

  const float pixelDx =
      (ndcEnd.x - ndcStart.x) * static_cast<float>(viewportWidth) * 0.5f;
  const float pixelDy =
      (ndcEnd.y - ndcStart.y) * static_cast<float>(viewportHeight) * 0.5f;
  const float pixelLength = std::sqrt(pixelDx * pixelDx + pixelDy * pixelDy);
  if (pixelLength < 1.0e-3f) {
    return false;
  }

  const float pixelDirX = pixelDx / pixelLength;
  const float pixelDirY = pixelDy / pixelLength;
  const float pixelPerpX = -pixelDirY;
  const float pixelPerpY = pixelDirX;

  const float halfWidthPixels = lineWidthPixels * 0.5f;
  const float ndcOffsetX =
      pixelPerpX * halfWidthPixels * 2.0f / static_cast<float>(viewportWidth);
  const float ndcOffsetY =
      pixelPerpY * halfWidthPixels * 2.0f / static_cast<float>(viewportHeight);

  const Math::Vector2 ndcCorners[4] = {
      Math::Vector2{ndcStart.x - ndcOffsetX, ndcStart.y - ndcOffsetY},
      Math::Vector2{ndcStart.x + ndcOffsetX, ndcStart.y + ndcOffsetY},
      Math::Vector2{ndcEnd.x - ndcOffsetX, ndcEnd.y - ndcOffsetY},
      Math::Vector2{ndcEnd.x + ndcOffsetX, ndcEnd.y + ndcOffsetY},
  };

  for (size_t i = 0; i < outPositions.size(); ++i) {
    if (!WorldPointOnFloorFromNdc(ndcCorners[i], cameraWorldPosition, viewMatrix,
                                perspectiveMatrix, floorY, outPositions[i])) {
      return false;
    }
  }

  outNormal = Math::Vector3::UpLh();
  return true;
}

} // namespace Atrum::Renderer
