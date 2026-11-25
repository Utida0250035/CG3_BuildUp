#include "System/AtomF/CmpAtomFFloor.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/VertexData.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kTwoPi = 6.28318530f;
const Math::Vector4 kFloorColor{0.34f, 0.36f, 0.32f, 1.0f};
const Math::Vector3 kUpNormal{0.0f, 1.0f, 0.0f};

VertexData MakeFloorVertex(const float x, const float y, const float z) {
  VertexData vertex{};
  vertex.position = {x, y, z, 1.0f};
  vertex.texCoord = {0.0f, 0.0f};
  vertex.normal = kUpNormal;
  return vertex;
}

void DrawFloorTriangle(AtrumEngine *engine, const Math::Matrix4x4 &world,
                       const VertexData &a, const VertexData &b,
                       const VertexData &c) {
  engine->DrawTriangle(WhiteTextureIndex(), kFloorColor, Math::Transform{},
                       world, std::array<VertexData, 3>{a, b, c}, false);
}

void DrawCircularFloor(AtrumEngine *engine, const Math::Matrix4x4 &world) {
  const float floorRadius = AtomF::ResolveArenaInnerRadiusMeters();
  const float groundY = AtomF::ResolveArenaGroundHeightMeters();
  const int segmentCount = std::max(3, AtomF::ResolveArenaMeshSegmentCount());
  const VertexData center = MakeFloorVertex(0.0f, groundY, 0.0f);
  for (int segment = 0; segment < segmentCount; ++segment) {
    const float angle0 =
        (kTwoPi * static_cast<float>(segment)) /
        static_cast<float>(segmentCount);
    const float angle1 = (kTwoPi * static_cast<float>(segment + 1)) /
                         static_cast<float>(segmentCount);
    const VertexData edge0 = MakeFloorVertex(floorRadius * std::cos(angle0),
                                             groundY,
                                             floorRadius * std::sin(angle0));
    const VertexData edge1 = MakeFloorVertex(floorRadius * std::cos(angle1),
                                             groundY,
                                             floorRadius * std::sin(angle1));
    DrawFloorTriangle(engine, world, center, edge0, edge1);
    DrawFloorTriangle(engine, world, center, edge1, edge0);
  }
}

} // namespace

void CmpAtomFFloor::Draw() {
  const Entity *owner = GetOwner();
  AtrumEngine *engine = AtrumEngine::Instance();
  if (owner == nullptr || engine == nullptr) {
    return;
  }

  DrawCircularFloor(engine, owner->GetWorldMatrix());
}

} // namespace Atrum