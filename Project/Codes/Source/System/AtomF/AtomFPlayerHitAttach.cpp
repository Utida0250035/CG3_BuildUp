#include "System/AtomF/AtomFPlayerHitAttach.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"

namespace Atrum::AtomF {

void AttachPlayerHitSphere(Entity &entity) {
  const float hitRadiusMeters = ResolvePlayerBodyRadiusMeters();

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