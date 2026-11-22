#include "System/AtomF/AtomFArenaContainmentAttach.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFArenaContainment.h"
#include "System/Entity/Entity.h"

namespace Atrum::AtomF {

void AttachArenaContainment(Entity &entity, const float bodyRadiusMeters) {
  CmpAtomFArenaContainment *existing =
      entity.GetUpdCmp<CmpAtomFArenaContainment>();
  if (existing != nullptr) {
    existing->SetBodyRadiusMeters(bodyRadiusMeters);
    return;
  }

  entity.AddUpdCmp<CmpAtomFArenaContainment>();
  CmpAtomFArenaContainment *containment =
      entity.GetUpdCmp<CmpAtomFArenaContainment>();
  if (containment != nullptr) {
    containment->SetBodyRadiusMeters(bodyRadiusMeters);
  }
}

void AttachArenaContainmentForPlayer(Entity &entity) {
  AttachArenaContainment(entity, ResolvePlayerBodyRadiusMeters());
}

void AttachArenaContainmentForPlayerBullet(Entity &entity) {
  AttachArenaContainment(entity, ResolvePlayerBulletRadiusMeters());
}

void AttachArenaContainmentForEnemy(Entity &entity) {
  AttachArenaContainment(entity, ResolveEnemyBodyRadiusMeters());
}

} // namespace Atrum::AtomF