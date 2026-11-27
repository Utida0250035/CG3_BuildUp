#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Entity/HitJudge.h"

#include <cmath>

namespace Atrum::AtomF {

/// 爆心地を中心とする半径cの球と、敵側の各 HitSphere が交差するかを判定する。
bool IsExplosionSphereIntersectingTarget(const Math::Vector3 &centerWorld,
                                         const float explosionRadiusMeters,
                                         const Entity &target,
                                         const CmpHitSphere &targetHitSphere) {
  if (explosionRadiusMeters <= 0.0f) {
    return false;
  }

  const Math::Vector3 targetBasePos = target.GetWorldPosition();
  for (const HitSphere &sphere : targetHitSphere.GetSpheres()) {
    if (sphere.radius < 0.0f) {
      continue;
    }
    const Math::Vector3 sphereCenter = targetBasePos + sphere.localOffset;
    const float sumRadius = explosionRadiusMeters + sphere.radius;
    if ((sphereCenter - centerWorld).LengthSquare() <= sumRadius * sumRadius) {
      return true;
    }
  }
  return false;
}

void SpreadEnemyExplosionKnockbackFromCenter(
    const Entity &explodingEnemy, const Math::Vector3 &centerWorld) {
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
  const float speed = ResolveEnemyKnockbackSpeedMetersPerSecond();

  for (CmpHitSphere *hitSphere : judge->RegisteredHitSpheres()) {
    if (hitSphere == nullptr || !hitSphere->GetIsActive()) {
      continue;
    }
    Entity *other = &hitSphere->RefOwner();
    if (other == &explodingEnemy || !other->IsActive()) {
      continue;
    }
    if (!tags.EntityHasRole(*other, "enemy")) {
      continue;
    }
    // ボスは爆風で吹き飛ばない（ダメージは ApplyEnemyExplosionDamageToBoss 側）。
    if (other->HasUpdCmp<CmpAtomFBoss>()) {
      continue;
    }
    if (other->HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
      if (CmpAtomFEnemyChainExplosion *chain =
              other->GetUpdCmp<CmpAtomFEnemyChainExplosion>();
          chain != nullptr && chain->IsDying()) {
        continue;
      }
    }

    if (!IsExplosionSphereIntersectingTarget(centerWorld, radiusMeters, *other,
                                             *hitSphere)) {
      continue;
    }

    CmpAtomFEnemyKnockback *knockback = nullptr;
    if (other->HasUpdCmp<CmpAtomFEnemyKnockback>()) {
      knockback = other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    } else {
      other->AddUpdCmp<CmpAtomFEnemyKnockback>();
      other->ResolveDependencies();
      knockback = other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    }
    if (knockback == nullptr) {
      continue;
    }
    knockback->ApplyKnockbackAwayFromExplosionCenter(centerWorld, speed);
  }
}

} // namespace Atrum::AtomF