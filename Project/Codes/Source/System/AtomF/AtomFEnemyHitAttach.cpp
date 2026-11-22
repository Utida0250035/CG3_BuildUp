#include "System/AtomF/AtomFEnemyHitAttach.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"

namespace Atrum::AtomF {

void AttachEnemyHitSphere(Entity &entity) {
  const float hitRadiusMeters = ResolveEnemyBodyRadiusMeters();

  CmpHitSphere *existing = entity.GetUpdCmp<CmpHitSphere>();
  if (existing != nullptr) {
    existing->SetSpheres({{{}, hitRadiusMeters}});
    return;
  }

  entity.AddUpdCmp<CmpHitSphere>();
  CmpHitSphere *hit = entity.GetUpdCmp<CmpHitSphere>();
  if (hit != nullptr) {
    hit->SetSpheres({{{}, hitRadiusMeters}});
  }
}

} // namespace Atrum::AtomF