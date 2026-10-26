#include "System/Editor/Particle/ParticleEditorReferenceGrid.h"

#include "Engine/AtrumEngine.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/WhiteTexture.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr float kGridHalfExtent = 8.0f;
constexpr float kGridStep = 1.0f;
constexpr int kMajorLineEvery = 5;

constexpr float kMinorLineWidth = 0.012f;
constexpr float kMajorLineWidth = 0.022f;
constexpr float kPlaneGridLineWidth = 0.01f;
constexpr float kAxisLineWidth = 0.045f;
constexpr float kOriginTickLength = 0.35f;
constexpr float kOriginTickWidth = 0.03f;

constexpr float kAxisHalfLength = kGridHalfExtent + 1.0f;

const Math::Vector4 kMinorGridColor{0.32f, 0.34f, 0.38f, 0.45f};
const Math::Vector4 kMajorGridColor{0.48f, 0.5f, 0.55f, 0.65f};
const Math::Vector4 kYzPlaneGridColor{0.28f, 0.32f, 0.36f, 0.28f};
const Math::Vector4 kXyPlaneGridColor{0.3f, 0.34f, 0.28f, 0.28f};

const Math::Vector4 kAxisColorX{0.95f, 0.28f, 0.28f, 0.95f};
const Math::Vector4 kAxisColorY{0.35f, 0.92f, 0.38f, 0.95f};
const Math::Vector4 kAxisColorZ{0.35f, 0.55f, 0.98f, 0.95f};
const Math::Vector4 kOriginColor{1.0f, 1.0f, 1.0f, 1.0f};

void DrawGridLine(AtrumEngine *engine, const Math::Vector3 &start,
                  const Math::Vector3 &end, const float width,
                  const Math::Vector4 &color) {
  engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, start, end,
                     width, false);
}

bool IsMajorGridIndex(const float coord) {
  const int index = static_cast<int>(std::lround(coord / kGridStep));
  return index % kMajorLineEvery == 0;
}

void DrawFloorGridXZ(AtrumEngine *engine) {
  for (float x = -kGridHalfExtent; x <= kGridHalfExtent + 1.0e-4f;
       x += kGridStep) {
    const bool major = IsMajorGridIndex(x);
    DrawGridLine(engine, {x, 0.0f, -kGridHalfExtent}, {x, 0.0f, kGridHalfExtent},
                 major ? kMajorLineWidth : kMinorLineWidth,
                 major ? kMajorGridColor : kMinorGridColor);
  }
  for (float z = -kGridHalfExtent; z <= kGridHalfExtent + 1.0e-4f;
       z += kGridStep) {
    const bool major = IsMajorGridIndex(z);
    DrawGridLine(engine, {-kGridHalfExtent, 0.0f, z}, {kGridHalfExtent, 0.0f, z},
                 major ? kMajorLineWidth : kMinorLineWidth,
                 major ? kMajorGridColor : kMinorGridColor);
  }
}

void DrawWallGridYZ(AtrumEngine *engine) {
  for (float y = -kGridHalfExtent; y <= kGridHalfExtent + 1.0e-4f;
       y += kGridStep) {
    DrawGridLine(engine, {0.0f, y, -kGridHalfExtent},
                 {0.0f, y, kGridHalfExtent}, kPlaneGridLineWidth,
                 kYzPlaneGridColor);
  }
  for (float z = -kGridHalfExtent; z <= kGridHalfExtent + 1.0e-4f;
       z += kGridStep) {
    DrawGridLine(engine, {0.0f, -kGridHalfExtent, z},
                 {0.0f, kGridHalfExtent, z}, kPlaneGridLineWidth,
                 kYzPlaneGridColor);
  }
}

void DrawWallGridXY(AtrumEngine *engine) {
  for (float x = -kGridHalfExtent; x <= kGridHalfExtent + 1.0e-4f;
       x += kGridStep) {
    DrawGridLine(engine, {x, -kGridHalfExtent, 0.0f},
                 {x, kGridHalfExtent, 0.0f}, kPlaneGridLineWidth,
                 kXyPlaneGridColor);
  }
  for (float y = -kGridHalfExtent; y <= kGridHalfExtent + 1.0e-4f;
       y += kGridStep) {
    DrawGridLine(engine, {-kGridHalfExtent, y, 0.0f},
                 {kGridHalfExtent, y, 0.0f}, kPlaneGridLineWidth,
                 kXyPlaneGridColor);
  }
}

void DrawRgbAxes(AtrumEngine *engine) {
  DrawGridLine(engine, {-kAxisHalfLength, 0.0f, 0.0f},
               {kAxisHalfLength, 0.0f, 0.0f}, kAxisLineWidth, kAxisColorX);
  DrawGridLine(engine, {0.0f, -kAxisHalfLength, 0.0f},
               {0.0f, kAxisHalfLength, 0.0f}, kAxisLineWidth, kAxisColorY);
  DrawGridLine(engine, {0.0f, 0.0f, -kAxisHalfLength},
               {0.0f, 0.0f, kAxisHalfLength}, kAxisLineWidth, kAxisColorZ);
}

void DrawOriginMarker(AtrumEngine *engine) {
  DrawGridLine(engine, {-kOriginTickLength, 0.0f, 0.0f},
               {kOriginTickLength, 0.0f, 0.0f}, kOriginTickWidth, kOriginColor);
  DrawGridLine(engine, {0.0f, -kOriginTickLength, 0.0f},
               {0.0f, kOriginTickLength, 0.0f}, kOriginTickWidth, kOriginColor);
  DrawGridLine(engine, {0.0f, 0.0f, -kOriginTickLength},
               {0.0f, 0.0f, kOriginTickLength}, kOriginTickWidth, kOriginColor);
}

} // namespace

void DrawParticleEditorReferenceGrid(AtrumEngine *engine) {
  if (engine == nullptr) {
    return;
  }

  // 奥行きの手がかり：XZ 床 → YZ / XY の薄い壁 → 軸 → 原点
  DrawWallGridXY(engine);
  DrawWallGridYZ(engine);
  DrawFloorGridXZ(engine);
  DrawRgbAxes(engine);
  DrawOriginMarker(engine);
}

} // namespace Atrum
