#include "System/Entity/Components/CmpDrawHitSphere.h"
#include "Engine/AtrumEngine.h"
#include "Math/Transform.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpDrawHitSphere::ResolveDependence() {
  cmpHitSphere_ = RefOwner().GetUpdCmp<CmpHitSphere>();
}

void CmpDrawHitSphere::Draw() {
  if (!cmpHitSphere_->GetIsActive()) {
    return;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  const Math::Vector3 origin = GetOwner()->GetWorldPosition();

  for (const HitSphere &hitSphere : cmpHitSphere_->GetSpheres()) {
    Math::TransformLH transform = RefOwner().GetTransform();
    transform.translate = origin + hitSphere.localOffset;
    engine->DrawSphere(WhiteTextureIndex(), colorVec_, Math::Transform{},
                       transform.MakeWorldMatrix(), hitSphere.radius, 12,
                       isLightingEnabled_);
  }
}

} // namespace Atrum
