#include "System/AtomF/AtomFPlayerBulletConsume.h"

#include "System/AtomF/CmpAtomFPlayerBulletTrail.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"

namespace Atrum::AtomF {

void ConsumePlayerBulletAfterHit(Entity &bullet) {
  if (bullet.HasUpdCmp<CmpHitSphere>()) {
    if (CmpHitSphere *hit = bullet.GetUpdCmp<CmpHitSphere>()) {
      hit->SetIsActive(false);
    }
  }
  if (CmpAtomFPlayerBulletTrail *trail =
          bullet.GetUpdCmp<CmpAtomFPlayerBulletTrail>()) {
    trail->BeginLingerPhase();
    return;
  }
  bullet.Finalize();
  bullet.RequestDestroy();
}

} // namespace Atrum::AtomF