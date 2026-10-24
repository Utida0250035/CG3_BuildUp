#include "System/BossCombatCleanup.h"

#include "System/CmpAttackTelegraph.h"
#include "System/CmpReflectAbleBullet.h"
#include "System/Entity/Entity.h"

namespace Atrum {

void ClearBossAttackTelegraphAndBullets(Entity &boss) {
  if (boss.HasDataCmp<CmpAttackTelegraph>()) {
    boss.GetDataCmp<CmpAttackTelegraph>()->Clear();
  }

  if (!boss.HasUpdCmp<CmpReflectAbleBullet>()) {
    return;
  }

  CmpReflectAbleBullet *cmpReflectAbleBullet =
      boss.GetUpdCmp<CmpReflectAbleBullet>();
  for (std::unique_ptr<Entity> &bullet : cmpReflectAbleBullet->RefBullets()) {
    if (bullet) {
      bullet->Finalize();
    }
  }
  cmpReflectAbleBullet->RefBullets().clear();
}

} // namespace Atrum
