#include "System/AtomF/AtomFEnemyExplosionPlayerDamage.h"

#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"

namespace Atrum::AtomF {

void ApplyEnemyExplosionDamageToPlayer(const Math::Vector3 &centerWorld) {
  if (!IsEnemyExplosionHitShapeSphere()) {
    return;
  }

  Entity *player = ScenePlayerLocator::Instance().PlayerEntity();
  if (player == nullptr || !player->IsActive()) {
    return;
  }

  CmpHitSphere *hitSphere = player->GetUpdCmp<CmpHitSphere>();
  CmpHealthHit *health = player->GetUpdCmp<CmpHealthHit>();
  if (hitSphere == nullptr || health == nullptr || !hitSphere->GetIsActive()) {
    return;
  }

  if (!IsExplosionSphereIntersectingTarget(
          centerWorld, ResolveEnemyExplosionHitRadiusMeters(), *player,
          *hitSphere)) {
    return;
  }
  health->TryTakeDamageUnconditional();
}

void ApplyEnemyExplosionDamageToBoss(const Math::Vector3 &centerWorld) {
  CmpAtomFBoss *boss = CmpAtomFBoss::Current();
  if (boss == nullptr || !boss->IsDamageable()) {
    return;
  }
  Entity &bossEntity = boss->RefOwner();
  if (!bossEntity.HasUpdCmp<CmpHitSphere>()) {
    return;
  }
  CmpHitSphere *hitSphere = bossEntity.GetUpdCmp<CmpHitSphere>();
  if (hitSphere == nullptr || !hitSphere->GetIsActive()) {
    return;
  }
  if (!IsExplosionSphereIntersectingTarget(
          centerWorld, ResolveEnemyExplosionHitRadiusMeters(), bossEntity,
          *hitSphere)) {
    return;
  }
  boss->ApplyDamage(ParametersRuntime::Instance().Boss().explosionDamage,
                    centerWorld);
}

} // namespace Atrum::AtomF
