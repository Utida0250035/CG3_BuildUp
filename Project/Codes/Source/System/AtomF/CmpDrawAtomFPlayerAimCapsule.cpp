#include "System/AtomF/CmpDrawAtomFPlayerAimCapsule.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/CmpAtomFPlayerAimCapsule.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpDrawAtomFPlayerAimCapsule::ResolveDependence() {
  capsule_ = RefOwner().GetUpdCmp<CmpAtomFPlayerAimCapsule>();
}

void CmpDrawAtomFPlayerAimCapsule::Draw() {
  if (capsule_ == nullptr || capsule_->CurrentLengthMeters() <= 1e-4f) {
    return;
  }

  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  const Math::Vector3 start = capsule_->CapsuleStartWorld();
  const Math::Vector3 end = capsule_->CapsuleEndWorld();
  const Math::Vector4 color{0.35f, 0.75f, 1.0f, 0.55f};
  engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, start, end,
                     0.06f);
}

} // namespace Atrum