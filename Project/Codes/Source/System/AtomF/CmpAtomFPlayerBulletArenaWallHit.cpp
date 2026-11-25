#include "System/AtomF/CmpAtomFPlayerBulletArenaWallHit.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFPlayerBulletConsume.h"
#include "System/AtomF/CmpAtomFPlayerBulletTrail.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum {

void CmpAtomFPlayerBulletArenaWallHit::ResolveDependence() {
  trail_ = RefOwner().GetUpdCmp<CmpAtomFPlayerBulletTrail>();
  tags_ = &EntityTagRegistry::Instance();
  tags_->EnsureLoaded();
}

void CmpAtomFPlayerBulletArenaWallHit::Update() {
  if (consumed_ || tags_ == nullptr) {
    return;
  }

  if (trail_ != nullptr && trail_->IsLingerPhase()) {
    return;
  }

  if (!tags_->EntityHasRole(RefOwner(), "playerWeapon")) {
    return;
  }

  const Math::Vector3 position = RefOwner().GetWorldPosition();
  if (!AtomF::IsWorldXZTouchingArenaWall(
          position.x, position.z,
          AtomF::ResolvePlayerBulletRadiusMeters())) {
    return;
  }

  consumed_ = true;
  AtomF::ConsumePlayerBulletAfterHit(RefOwner());
}

} // namespace Atrum