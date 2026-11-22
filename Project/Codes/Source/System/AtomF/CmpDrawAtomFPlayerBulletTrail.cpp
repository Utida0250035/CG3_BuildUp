#include "System/AtomF/CmpDrawAtomFPlayerBulletTrail.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Math/Transform.h"
#include "Math/Vector4.h"
#include "System/AtomF/CmpAtomFPlayerBulletTrail.h"
#include "System/Entity/Entity.h"
#include "System/WhiteTexture.h"

namespace Atrum {

void CmpDrawAtomFPlayerBulletTrail::ResolveDependence() {
  trail_ = RefOwner().GetUpdCmp<CmpAtomFPlayerBulletTrail>();
}

void CmpDrawAtomFPlayerBulletTrail::Draw() {
  if (trail_ == nullptr) {
    return;
  }

  const std::vector<AtomFPlayerBulletTrailSample> &samples = trail_->Samples();
  if (samples.size() < 2) {
    return;
  }

  AtrumEngine *engine = EngineInstance();
  if (engine == nullptr) {
    return;
  }

  const float duration = trail_->TrailDurationSeconds();
  if (duration <= 1e-6f) {
    return;
  }

  const Math::Vector4 baseColor{0.95f, 0.85f, 0.35f, 1.0f};

  for (size_t index = 1; index < samples.size(); ++index) {
    const AtomFPlayerBulletTrailSample &prev = samples[index - 1];
    const AtomFPlayerBulletTrailSample &current = samples[index];

    const float ageMid = (prev.ageSeconds + current.ageSeconds) * 0.5f;
    const float fade = 1.0f - (ageMid / duration);
    if (fade <= 0.0f) {
      continue;
    }

    Math::Vector4 color = baseColor;
    color.w *= fade;
    engine->DrawLine3D(WhiteTextureIndex(), color, Math::Transform{},
                       prev.worldPosition, current.worldPosition, 0.02f);
  }
}

} // namespace Atrum