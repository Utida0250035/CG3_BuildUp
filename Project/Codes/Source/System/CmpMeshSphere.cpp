#include "System/CmpMeshSphere.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Math/Transform.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpMeshSphere::Draw() {
  AtrumEngine *engine = AtrumEngine::Instance();
  const float scale = RefOwner().GetTransform().scale.x;
  const bool useAlphaBlend = colorVec_.w < 0.999f;
  if (useAlphaBlend) {
    engine->SetBlendModeForFrame(BlendMode::NORMAL);
  }
  engine->DrawSphere(
      WhiteTextureIndex(), colorVec_, Math::Transform{},
      GetOwner()->GetWorldMatrix(), radius_ * scale, subdivision_,
      isLightingEnabled_);
  if (useAlphaBlend) {
    engine->SetBlendModeForFrame(BlendMode::NONE);
  }
}

} // namespace Atrum
