#include "System/Editor/EditorInfiniteGrid.h"

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/WhiteTexture.h"

#include <algorithm>
#include <cmath>

namespace Atrum::Editor {

namespace {

/// Z ファイティング回避のわずかな持ち上げ
constexpr float kFloorY = 0.05f;
/// ワールド単位の線幅（画面ピクセル換算は一旦使わない）
constexpr float kMinorLineWidth = 0.012f;
constexpr float kMajorLineWidth = 0.022f;
constexpr float kAxisLineWidth = 0.045f;
/// マイナー1本あたりの画面上の目安（Unity Scene View 相当）
constexpr float kTargetPixelsPerMinorLine = 40.0f;
constexpr int kMajorLinesPerMinorBlock = 10;
constexpr float kDrawExtentMargin = 1.2f;

const Math::Vector4 kMinorGridColor{0.62f, 0.64f, 0.68f, 0.72f};
const Math::Vector4 kMajorGridColor{0.78f, 0.8f, 0.84f, 0.92f};
const Math::Vector4 kAxisColorX{0.95f, 0.32f, 0.32f, 0.98f};
const Math::Vector4 kAxisColorZ{0.38f, 0.58f, 0.98f, 0.98f};

void DrawGridLine(AtrumEngine *engine, const Math::Vector3 &start,
                  const Math::Vector3 &end, const float lineWidth,
                  const Math::Vector4 &color) {
  engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, start, end,
                     lineWidth, false);
}

bool IsNearlyZero(const float value, const float epsilon) {
  return std::abs(value) < epsilon;
}

bool IsMajorWorldLine(const float worldCoord, const float majorStep) {
  if (majorStep <= 0.0f) {
    return false;
  }
  const float ratio = worldCoord / majorStep;
  const float nearest = std::round(ratio);
  return std::abs(ratio - nearest) < 1.0e-3f;
}

float SnapUnityStyleGridSize(const float worldSize) {
  if (worldSize <= 0.0f) {
    return 1.0f;
  }
  const float exponent = std::floor(std::log10(worldSize));
  const float magnitude = std::pow(10.0f, exponent);
  const float normalized = worldSize / magnitude;
  float mantissa = 1.0f;
  if (normalized >= 5.0f) {
    mantissa = 5.0f;
  } else if (normalized >= 2.0f) {
    mantissa = 2.0f;
  }
  return mantissa * magnitude;
}

float EstimateFloorVisibleHalfExtent(const Camera *camera) {
  const Math::Vector3 camPos = camera->GetTranslate();
  const Math::Matrix4x4 &view = camera->GetViewMatrix();
  const Math::Matrix4x4 &proj = camera->GetPerspectiveFovMatrix();

  Math::Vector3 forward{view.m[2][0], view.m[2][1], view.m[2][2]};
  if (forward.LengthSquare() < 1.0e-8f) {
    return 48.0f;
  }
  forward.Normalize();

  float viewDistance = std::max(2.0f, std::abs(camPos.y) + 2.0f);
  if (std::abs(forward.y) > 1.0e-5f) {
    const float t = (kFloorY - camPos.y) / forward.y;
    if (t > 0.1f) {
      viewDistance = t;
    }
  }

  const float cot = proj.m[1][1];
  const float tanHalfFovY = (cot > 1.0e-6f) ? (1.0f / cot) : std::tan(0.25f);
  const float aspect =
      (std::abs(proj.m[0][0]) > 1.0e-6f) ? (proj.m[1][1] / proj.m[0][0])
                                         : (16.0f / 9.0f);

  const float halfVert = viewDistance * tanHalfFovY;
  const float halfHoriz = halfVert * aspect;

  const Math::Vector3 focus = camPos + forward * viewDistance;
  const float deltaX = focus.x - camPos.x;
  const float deltaZ = focus.z - camPos.z;
  const float horizOffset = std::sqrt(deltaX * deltaX + deltaZ * deltaZ);

  float extent = std::max(halfHoriz, halfVert) + horizOffset * 0.35f;
  extent = std::max(extent, viewDistance * 0.25f);
  return std::clamp(extent, 4.0f, 5000.0f);
}

float ComputeMinorGridStep(const float visibleHalfExtent, const int viewportWidth,
                           const int viewportHeight) {
  if (viewportWidth > 0 && viewportHeight > 0) {
    const float viewportSpan =
        static_cast<float>(std::min(viewportWidth, viewportHeight));
    const float worldSpan = visibleHalfExtent * 2.0f;
    const float metersPerPixel = worldSpan / viewportSpan;
    const float rawMinor = metersPerPixel * kTargetPixelsPerMinorLine;
    return SnapUnityStyleGridSize(rawMinor);
  }
  return SnapUnityStyleGridSize(visibleHalfExtent / 16.0f);
}

struct GridLineStyle {
  float width = kMinorLineWidth;
  Math::Vector4 color = kMinorGridColor;
};

GridLineStyle StyleForParallelToZLine(const float x, const float minorStep,
                                      const float majorStep) {
  const float axisEpsilon = minorStep * 0.05f;
  GridLineStyle style{kMinorLineWidth, kMinorGridColor};
  if (IsNearlyZero(x, axisEpsilon)) {
    style = {kAxisLineWidth, kAxisColorZ};
  } else if (IsMajorWorldLine(x, majorStep)) {
    style = {kMajorLineWidth, kMajorGridColor};
  }
  return style;
}

GridLineStyle StyleForParallelToXLine(const float z, const float minorStep,
                                      const float majorStep) {
  const float axisEpsilon = minorStep * 0.05f;
  GridLineStyle style{kMinorLineWidth, kMinorGridColor};
  if (IsNearlyZero(z, axisEpsilon)) {
    style = {kAxisLineWidth, kAxisColorX};
  } else if (IsMajorWorldLine(z, majorStep)) {
    style = {kMajorLineWidth, kMajorGridColor};
  }
  return style;
}

void DrawUnityStyleFloorGrid(AtrumEngine *engine, const Math::Vector3 &camPos,
                             const float drawExtent, const float minorStep) {
  const float majorStep = minorStep *
                          static_cast<float>(kMajorLinesPerMinorBlock);

  const float minX = camPos.x - drawExtent;
  const float maxX = camPos.x + drawExtent;
  const float minZ = camPos.z - drawExtent;
  const float maxZ = camPos.z + drawExtent;

  const float startX = std::floor(minX / minorStep) * minorStep;
  const float startZ = std::floor(minZ / minorStep) * minorStep;

  for (float x = startX; x <= maxX + minorStep * 0.5f; x += minorStep) {
    const GridLineStyle style = StyleForParallelToZLine(x, minorStep, majorStep);
    DrawGridLine(engine, {x, kFloorY, minZ}, {x, kFloorY, maxZ}, style.width,
                 style.color);
  }

  for (float z = startZ; z <= maxZ + minorStep * 0.5f; z += minorStep) {
    const GridLineStyle style = StyleForParallelToXLine(z, minorStep, majorStep);
    DrawGridLine(engine, {minX, kFloorY, z}, {maxX, kFloorY, z}, style.width,
                 style.color);
  }
}

} // namespace

void DrawInfiniteEditorFloorGrid(AtrumEngine *engine, const Camera *camera,
                                 const int viewportWidth,
                                 const int viewportHeight) {
  if (engine == nullptr || camera == nullptr) {
    return;
  }

  const Math::Vector3 camPos = camera->GetTranslate();
  const float visibleHalfExtent = EstimateFloorVisibleHalfExtent(camera);
  const float minorStep = ComputeMinorGridStep(
      visibleHalfExtent, std::max(viewportWidth, 1), std::max(viewportHeight, 1));
  const float drawExtent = visibleHalfExtent * kDrawExtentMargin;

  DrawUnityStyleFloorGrid(engine, camPos, drawExtent, minorStep);
}

} // namespace Atrum::Editor
