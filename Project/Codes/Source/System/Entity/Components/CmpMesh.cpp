#include "System/Entity/Components/CmpMesh.h"
#include "Engine/AtrumEngine.h"
#include "Math/Transform.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpMesh::Initialize() {}

void CmpMesh::Draw() {
  AtrumEngine *engine = AtrumEngine::Instance();
  const Math::Matrix4x4 worldMatrix = GetOwner()->GetWorldMatrix();

  if (model_) {
    engine->DrawModel(model_, worldMatrix, isLightingEnabled_, color_);
    return;
  }

  engine->DrawSphere(WhiteTextureIndex(), color_, Math::Transform{}, worldMatrix,
                     radius_, 8, isLightingEnabled_);
}

} // namespace Atrum