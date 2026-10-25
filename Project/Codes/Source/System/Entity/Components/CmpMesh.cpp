#include "System/Entity/Components/CmpMesh.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpMesh::Initialize() {}

void CmpMesh::Draw() {
  AtrumEngine *engine = AtrumEngine::Instance();
  const Math::Matrix4x4 worldMatrix = GetOwner()->GetWorldMatrix();
  const bool useAlphaBlend = color_.w < 0.999f;
  if (useAlphaBlend) {
    engine->SetBlendModeForFrame(BlendMode::NORMAL);
  }

  if (model_) {
    engine->DrawModel(model_, worldMatrix, isLightingEnabled_, color_);
  } else {
    engine->DrawSphere(WhiteTextureIndex(), color_, Math::Transform{},
                       worldMatrix, radius_, 8, isLightingEnabled_);
  }

  if (useAlphaBlend) {
    engine->SetBlendModeForFrame(BlendMode::NONE);
  }
}

} // namespace Atrum