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

constexpr float kFloorY = 0.05f;
constexpr float kGridStep = 1.0f;
constexpr int kMajorLineEvery = 10;
constexpr int kMaxLinesPerAxis = 48;
constexpr float kDefaultHalfExtent = 40.0f;

constexpr float kMinorLineWidth = 0.018f;
constexpr float kMajorLineWidth = 0.032f;
constexpr float kAxisLineWidth = 0.055f;

const Math::Vector4 kMinorGridColor{0.58f, 0.6f, 0.64f, 0.85f};
const Math::Vector4 kMajorGridColor{0.75f, 0.77f, 0.81f, 0.95f};
const Math::Vector4 kAxisColorX{0.95f, 0.32f, 0.32f, 0.98f};
const Math::Vector4 kAxisColorZ{0.38f, 0.58f, 0.98f, 0.98f};

void DrawFloorLine(AtrumEngine *engine, const Math::Vector3 &start,
                   const Math::Vector3 &end, const float width,
                   const Math::Vector4 &color) {
  engine->DrawLine3DOnFloor(WhiteTextureIndex(), color, Math::Transform{}, start,
                            end, width, false);
}

float ComputeHalfExtent(const Math::Vector3 &camPos) {
  const float fromHeight = std::max(std::abs(camPos.y), 4.0f) * 2.5f + 12.0f;
  return std::clamp(fromHeight, kDefaultHalfExtent, 96.0f);
}

float ComputeGridStep(const float halfExtent) {
  float step = kGridStep;
  while (halfExtent * 2.0f / step >
         static_cast<float>(kMaxLinesPerAxis) + 0.5f) {
    step *= 2.0f;
  }
  return step;
}

bool IsOnWorldOriginAxis(const float worldCoord, const float step) {
  return std::abs(worldCoord) < step * 0.2f;
}

bool IsMajorGridLine(const float worldCoord, const float step) {
  const int index = static_cast<int>(std::lround(worldCoord / step));
  return index % kMajorLineEvery == 0;
}

/// 平行方向: worldCoord は X なら Z 方向ライン、Z なら X 方向ライン
void PickLineStyle(const float worldCoord, const float step,
                   const bool lineRunsAlongZ, float &outWidth,
                   Math::Vector4 &outColor) {
  outWidth = kMinorLineWidth;
  outColor = kMinorGridColor;

  if (IsOnWorldOriginAxis(worldCoord, step)) {
    outWidth = kAxisLineWidth;
    outColor = lineRunsAlongZ ? kAxisColorZ : kAxisColorX;
    return;
  }

  if (IsMajorGridLine(worldCoord, step)) {
    outWidth = kMajorLineWidth;
    outColor = kMajorGridColor;
  }
}

void DrawXZGrid(AtrumEngine *engine, const Math::Vector3 &camPos,
                const float halfExtent, const float step) {
  const float minX = camPos.x - halfExtent;
  const float maxX = camPos.x + halfExtent;
  const float minZ = camPos.z - halfExtent;
  const float maxZ = camPos.z + halfExtent;

  const float startX = std::floor(minX / step) * step;
  const float startZ = std::floor(minZ / step) * step;

  for (float x = startX; x <= maxX + step * 0.5f; x += step) {
    float width = kMinorLineWidth;
    Math::Vector4 color = kMinorGridColor;
    PickLineStyle(x, step, true, width, color);
    DrawFloorLine(engine, {x, kFloorY, minZ}, {x, kFloorY, maxZ}, width, color);
  }

  for (float z = startZ; z <= maxZ + step * 0.5f; z += step) {
    float width = kMinorLineWidth;
    Math::Vector4 color = kMinorGridColor;
    PickLineStyle(z, step, false, width, color);
    DrawFloorLine(engine, {minX, kFloorY, z}, {maxX, kFloorY, z}, width, color);
  }
}

} // namespace

void DrawInfiniteEditorFloorGrid(AtrumEngine *engine, const Camera *camera,
                                 const int viewportWidth,
                                 const int viewportHeight) {
  (void)viewportWidth;
  (void)viewportHeight;

  if (engine == nullptr || camera == nullptr) {
    return;
  }

  const Math::Vector3 camPos = camera->GetTranslate();
  const float halfExtent = ComputeHalfExtent(camPos);
  const float step = ComputeGridStep(halfExtent);
  DrawXZGrid(engine, camPos, halfExtent, step);
}

} // namespace Atrum::Editor
