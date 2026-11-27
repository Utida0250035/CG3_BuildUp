#include "System/AtomF/CmpAtomFBossShockwave.h"

#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpDrawAtomFTelegraph.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

void CmpAtomFBossShockwave::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

void CmpAtomFBossShockwave::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  radius_ += speed_ * frameDeltaTime_->GetDeltaTime();

  const float fade = 1.0f - std::min(1.0f, radius_ / maxRadius_);

  if (RefOwner().HasDrawCmp<CmpDrawAtomFTelegraph>()) {
    if (CmpDrawAtomFTelegraph *telegraph =
            RefOwner().GetDrawCmp<CmpDrawAtomFTelegraph>()) {
      telegraph->Clear();
      // 外縁（明るい細線）と帯（淡い太線）の二重の輪。
      telegraph->AddRing(center_, radius_, bandHalfWidth_ * 2.0f,
                    Math::Vector4{1.0f, 0.3f, 0.9f, 0.35f * fade + 0.1f});
      telegraph->AddRing(center_, radius_, bandHalfWidth_ * 0.7f,
                    Math::Vector4{1.0f, 0.9f, 1.0f, 0.9f * fade + 0.1f});
    }
  }

  if (!hasHitPlayer_) {
    Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
    if (player != nullptr && player->IsActive() &&
        player->HasUpdCmp<CmpHealthHit>()) {
      const Math::Vector3 playerPos = player->GetWorldPosition();
      const float dx = playerPos.x - center_.x;
      const float dz = playerPos.z - center_.z;
      const float dist = std::sqrt(dx * dx + dz * dz);
      const float reach =
          bandHalfWidth_ + AtomF::ResolvePlayerBodyRadiusMeters();
      if (std::fabs(dist - radius_) <= reach) {
        hasHitPlayer_ = true;
        if (CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>()) {
          health->TryTakeDamageUnconditional();
        }
      }
    }
  }

  if (radius_ >= maxRadius_) {
    RefOwner().Finalize();
    RefOwner().RequestDestroy();
  }
}

} // namespace Atrum
