#include "System/AtomF/CmpAtomFFloor.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/VertexData.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

#include <array>

namespace Atrum {

namespace {

constexpr float kFloorHalfExtentMeters = 32.0f;
const Math::Vector4 kFloorColor{0.34f, 0.36f, 0.32f, 1.0f};
const Math::Vector3 kUpNormal{0.0f, 1.0f, 0.0f};

VertexData MakeFloorVertex(const float x, const float z) {
  VertexData vertex{};
  vertex.position = {x, 0.0f, z, 1.0f};
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

} // namespace

void CmpAtomFFloor::Draw() {
  const Entity *owner = GetOwner();
  AtrumEngine *engine = AtrumEngine::Instance();
  if (owner == nullptr || engine == nullptr) {
    return;
  }

  const float extent = kFloorHalfExtentMeters;
  const VertexData leftNear = MakeFloorVertex(-extent, -extent);
  const VertexData leftFar = MakeFloorVertex(-extent, extent);
  const VertexData rightFar = MakeFloorVertex(extent, extent);
  const VertexData rightNear = MakeFloorVertex(extent, -extent);
  const Math::Matrix4x4 world = owner->GetWorldMatrix();

  // 表裏を描き、カメラが上からでも下からでも床面が見えるようにする。
  DrawFloorTriangle(engine, world, leftNear, leftFar, rightFar);
  DrawFloorTriangle(engine, world, leftNear, rightFar, rightNear);
  DrawFloorTriangle(engine, world, leftNear, rightFar, leftFar);
  DrawFloorTriangle(engine, world, leftNear, rightNear, rightFar);
}

} // namespace Atrum
