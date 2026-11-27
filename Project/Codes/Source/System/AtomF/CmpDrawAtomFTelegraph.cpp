#include "System/AtomF/CmpDrawAtomFTelegraph.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "System/WhiteTexture.h"

#include <cmath>

namespace Atrum {

namespace {

/// 床の上にわずかに浮かせて Z ファイティングを避ける。
constexpr float kTelegraphFloorY = 0.03f;
constexpr int kRingSegmentCount = 40;
constexpr float kTwoPi = 6.28318530718f;

} // namespace

void CmpDrawAtomFTelegraph::AddLine(const Math::Vector3 &start,
                                    const Math::Vector3 &end, const float width,
                                    const Math::Vector4 &color) {
  Shape shape{};
  shape.isRing = false;
  shape.a = start;
  shape.b = end;
  shape.width = width;
  shape.color = color;
  shapes_.push_back(shape);
}

void CmpDrawAtomFTelegraph::AddRing(const Math::Vector3 &center,
                                    const float radius, const float width,
                                    const Math::Vector4 &color) {
  Shape shape{};
  shape.isRing = true;
  shape.a = center;
  shape.radius = radius;
  shape.width = width;
  shape.color = color;
  shapes_.push_back(shape);
}

void CmpDrawAtomFTelegraph::Draw() {
  if (shapes_.empty()) {
    return;
  }
  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  const uint32_t texture = WhiteTextureIndex();
  const Math::Transform uvTransform{};
  for (const Shape &shape : shapes_) {
    if (!shape.isRing) {
      const Math::Vector3 start{shape.a.x, kTelegraphFloorY, shape.a.z};
      const Math::Vector3 end{shape.b.x, kTelegraphFloorY, shape.b.z};
      engine->DrawLine3DOnFloor(texture, shape.color, uvTransform, start, end,
                                shape.width);
      continue;
    }
    if (shape.radius <= 1e-4f) {
      continue;
    }
    for (int i = 0; i < kRingSegmentCount; ++i) {
      const float a0 = kTwoPi * static_cast<float>(i) / kRingSegmentCount;
      const float a1 = kTwoPi * static_cast<float>(i + 1) / kRingSegmentCount;
      const Math::Vector3 p0{shape.a.x + std::cos(a0) * shape.radius,
                             kTelegraphFloorY,
                             shape.a.z + std::sin(a0) * shape.radius};
      const Math::Vector3 p1{shape.a.x + std::cos(a1) * shape.radius,
                             kTelegraphFloorY,
                             shape.a.z + std::sin(a1) * shape.radius};
      engine->DrawLine3DOnFloor(texture, shape.color, uvTransform, p0, p1,
                                shape.width);
    }
  }
}

} // namespace Atrum
