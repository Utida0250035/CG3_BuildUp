#include "System/AtomF/AtomFEnemyExplosionKnockbackSpread.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Entity/HitJudge.h"

#include <cmath>

namespace Atrum::AtomF {

void SpreadEnemyExplosionKnockbackFromCenter(
    const Entity &explodingEnemy, const Math::Vector3 &centerWorld) {
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return;
  }

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const float radiusMeters = ResolveEnemyExplosionHitRadiusMeters();
  const float radiusSq = radiusMeters * radiusMeters;
  const float speed = ResolveEnemyKnockbackSpeedMetersPerSecond();

  for (CmpHitSphere *hitSphere : judge->RegisteredHitSpheres()) {
    if (hitSphere == nullptr) {
      continue;
    }
    Entity *other = &hitSphere->RefOwner();
    if (other == &explodingEnemy) {
      continue;
    }
    if (!tags.EntityHasRole(*other, "enemy")) {
      continue;
    }
    if (CmpAtomFEnemyChainExplosion *chain =
            other->GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      continue;
    }

    const Math::Vector3 otherPos = other->GetWorldPosition();
    Math::Vector3 delta = otherPos - centerWorld;
    delta.y = 0.0f;
    const float distSq = delta.x * delta.x + delta.z * delta.z;
    if (distSq > radiusSq) {
      continue;
    }

    CmpAtomFEnemyKnockback *knockback = other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    if (knockback == nullptr) {
      other->AddUpdCmp<CmpAtomFEnemyKnockback>();
      knockback = other->GetUpdCmp<CmpAtomFEnemyKnockback>();
    }
    if (knockback == nullptr) {
      continue;
    }
    knockback->ApplyKnockbackAwayFromExplosionCenter(centerWorld, speed);
  }
}

} // namespace Atrum::AtomF