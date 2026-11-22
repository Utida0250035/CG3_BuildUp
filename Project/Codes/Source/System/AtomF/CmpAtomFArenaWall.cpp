#include "System/AtomF/CmpAtomFArenaWall.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/VertexData.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <array>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kArenaInnerRadiusMeters = 16.0f;
constexpr float kWallThicknessMeters = 0.4f;
constexpr float kWallHeightMeters = 1.5f;
constexpr int kWallSegmentCount = 48;
constexpr float kTwoPi = 6.28318530f;
const Math::Vector4 kWallColor{0.75f, 0.62f, 0.38f, 1.0f};
const Math::Vector3 kUpNormal{0.0f, 1.0f, 0.0f};

VertexData MakeWallVertex(const float x, const float y, const float z) {
  VertexData vertex{};
  vertex.position = {x, y, z, 1.0f};
  vertex.texCoord = {0.0f, 0.0f};
  vertex.normal = kUpNormal;
  return vertex;
}

void DrawWallTriangle(AtrumEngine *engine, const Math::Matrix4x4 &world,
                      const VertexData &a, const VertexData &b,
                      const VertexData &c) {
  engine->DrawTriangle(WhiteTextureIndex(), kWallColor, Math::Transform{},
                       world, std::array<VertexData, 3>{a, b, c}, false);
}

void DrawWallQuad(AtrumEngine *engine, const Math::Matrix4x4 &world,
                  const VertexData &a, const VertexData &b,
                  const VertexData &c, const VertexData &d) {
  DrawWallTriangle(engine, world, a, b, c);
  DrawWallTriangle(engine, world, a, c, d);
  DrawWallTriangle(engine, world, a, c, b);
  DrawWallTriangle(engine, world, a, d, c);
}

void DrawArenaWallRing(AtrumEngine *engine, const Math::Matrix4x4 &world) {
  const float innerRadius = kArenaInnerRadiusMeters;
  const float outerRadius = kArenaInnerRadiusMeters + kWallThicknessMeters;
  const float height = kWallHeightMeters;
  for (int segment = 0; segment < kWallSegmentCount; ++segment) {
    const float angle0 =
        (kTwoPi * static_cast<float>(segment)) / static_cast<float>(kWallSegmentCount);
    const float angle1 = (kTwoPi * static_cast<float>(segment + 1)) /
                         static_cast<float>(kWallSegmentCount);
    const float cos0 = std::cos(angle0);
    const float sin0 = std::sin(angle0);
    const float cos1 = std::cos(angle1);
    const float sin1 = std::sin(angle1);

    const VertexData innerBottom0 = MakeWallVertex(innerRadius * cos0, 0.0f, innerRadius * sin0);
    const VertexData innerBottom1 = MakeWallVertex(innerRadius * cos1, 0.0f, innerRadius * sin1);
    const VertexData innerTop0 = MakeWallVertex(innerRadius * cos0, height, innerRadius * sin0);
    const VertexData innerTop1 = MakeWallVertex(innerRadius * cos1, height, innerRadius * sin1);
    const VertexData outerBottom0 = MakeWallVertex(outerRadius * cos0, 0.0f, outerRadius * sin0);
    const VertexData outerBottom1 = MakeWallVertex(outerRadius * cos1, 0.0f, outerRadius * sin1);
    const VertexData outerTop0 = MakeWallVertex(outerRadius * cos0, height, outerRadius * sin0);
    const VertexData outerTop1 = MakeWallVertex(outerRadius * cos1, height, outerRadius * sin1);

    DrawWallQuad(engine, world, innerBottom0, innerBottom1, innerTop1, innerTop0);
    DrawWallQuad(engine, world, outerBottom0, outerTop0, outerTop1, outerBottom1);
    DrawWallQuad(engine, world, innerTop0, innerTop1, outerTop1, outerTop0);
  }
}

} // namespace

void CmpAtomFArenaWall::Draw() {
  const Entity *owner = GetOwner();
  AtrumEngine *engine = AtrumEngine::Instance();
  if (owner == nullptr || engine == nullptr) {
    return;
  }

  DrawArenaWallRing(engine, owner->GetWorldMatrix());
}

} // namespace Atrum
