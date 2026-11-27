#include "System/AtomF/AtomFEnemyExplosionPlayerDamage.h"

#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/Entity/Components/CmpHealthHit.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Entity/HitJudge.h"

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
  if (!IsEnemyExplosionHitShapeSphere()) {
    return;
  }
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float radiusMeters = ResolveEnemyExplosionHitRadiusMeters();
  for (CmpHitSphere *hitSphere : judge->RegisteredHitSpheres()) {
    if (hitSphere == nullptr || !hitSphere->GetIsActive()) {
      continue;
    }
    Entity *other = &hitSphere->RefOwner();
    if (!other->IsActive() || !tags.EntityHasRole(*other, "boss")) {
      continue;
    }
    CmpAtomFBoss *boss = other->GetUpdCmp<CmpAtomFBoss>();
    if (boss == nullptr ||
        !IsExplosionSphereIntersectingTarget(centerWorld, radiusMeters, *other,
                                             *hitSphere)) {
      continue;
    }
    boss->ApplyExplosionHit();
  }
}

} // namespace Atrum::AtomF
