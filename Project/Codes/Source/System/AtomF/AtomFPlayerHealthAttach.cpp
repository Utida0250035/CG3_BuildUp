#include "System/AtomF/AtomFPlayerHealthAttach.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"

namespace Atrum::AtomF {

void AttachPlayerHealthHit(Entity &entity) {
  CmpHealthHit *existing = entity.GetUpdCmp<CmpHealthHit>();
  if (existing == nullptr) {
    entity.AddUpdCmp<CmpHealthHit>();
    existing = entity.GetUpdCmp<CmpHealthHit>();
  }
  if (existing == nullptr) {
    return;
  }

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  existing->SetHurterTagHashes(tags.PlayerDamageHurterTagHashes());
  existing->SetInitialHealthCount(99);
  existing->SetInvincibleTime(ResolvePlayerDamageVignetteSeconds());
}

} // namespace Atrum::AtomF