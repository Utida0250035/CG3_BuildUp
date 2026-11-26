#include "System/AtomF/CmpDrawAtomFPlayerBulletPredictCapsule.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/CmpAtomFPlayerBulletPredictCapsule.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpDrawAtomFPlayerBulletPredictCapsule::ResolveDependence() {
  capsule_ = RefOwner().GetUpdCmp<CmpAtomFPlayerBulletPredictCapsule>();
}

void CmpDrawAtomFPlayerBulletPredictCapsule::Draw() {
  if (capsule_ == nullptr || capsule_->CurrentLengthMeters() <= 1e-4f) {
    return;
  }

  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  const Math::Vector3 start = capsule_->CapsuleStartWorld();
  const Math::Vector3 end = capsule_->CapsuleEndWorld();
  const float radius = capsule_->RadiusMeters();
  const Math::Vector4 color{0.95f, 0.85f, 0.25f, 0.45f};
  // 弾本体の形が帯に隠れないよう、色のアルファどおり半透明で重ねる。
  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{}, start, end,
                     radius * 2.0f);
  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum
