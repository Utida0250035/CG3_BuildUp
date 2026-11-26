#include "System/AtomF/AtomFPlayerHealthAttach.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/PlaySe.h"

namespace Atrum::AtomF {

void AttachPlayerHealthHit(Entity &entity) {
  PreloadSeCandidates(
      {"./Assets/Audios/sePlayerDamage.mp3", "./Resources/sePlayerDamage.mp3"});

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
  existing->SetInitialHealthCount(ResolvePlayerInitialHealthCount());
  existing->SetInvincibleTime(ResolvePlayerInvincibilityParameters().seconds);
}

} // namespace Atrum::AtomF