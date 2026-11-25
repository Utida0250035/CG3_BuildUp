#include "System/AtomF/CmpAtomFEnemyKnockbackPredictCapsule.h"

#include "System/AtomF/AtomFArenaBounds.h"
#include "System/AtomF/AtomFEnemyPredictCapsuleTargetQuery.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFPlayerAimCapsuleRayClamp.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/AtomF/CmpAtomFEnemyKnockback.h"
#include "System/Entity/Entity.h"

namespace Atrum {

namespace {

float MaxPredictCapsuleLengthMeters() {
  return AtomF::ResolveArenaInnerRadiusMeters() * 2.0f;
}

} // namespace

void CmpAtomFEnemyKnockbackPredictCapsule::ResolveDependence() {
  radiusMeters_ = AtomF::ResolveEnemyBodyRadiusMeters();
}

Math::Vector3 CmpAtomFEnemyKnockbackPredictCapsule::CapsuleStartWorld() {
  return RefOwner().GetWorldPosition();
}

Math::Vector3 CmpAtomFEnemyKnockbackPredictCapsule::CapsuleEndWorld() {
  const Math::Vector3 start = CapsuleStartWorld();
  if (currentLengthMeters_ <= 0.0f) {
    return start;
  }
  return start + directionXZ_ * currentLengthMeters_;
}

void CmpAtomFEnemyKnockbackPredictCapsule::Update() {
  currentLengthMeters_ = 0.0f;
  radiusMeters_ = AtomF::ResolveEnemyBodyRadiusMeters();

  if (RefOwner().HasUpdCmp<CmpAtomFEnemyChainExplosion>()) {
    if (CmpAtomFEnemyChainExplosion *chain =
            RefOwner().GetUpdCmp<CmpAtomFEnemyChainExplosion>();
        chain != nullptr && chain->IsDying()) {
      return;
    }
  }
  if (RefOwner().HasUpdCmp<CmpAtomFEnemyKnockback>()) {
    if (CmpAtomFEnemyKnockback *knockback =
            RefOwner().GetUpdCmp<CmpAtomFEnemyKnockback>();
        knockback != nullptr && knockback->HasActiveKnockback()) {
      return;
    }
  }

  AtomF::EnemyPredictTargetInfo info{};
  if (!AtomF::TryResolveEnemyPredictTarget(RefOwner(), info) ||
      !info.targeted) {
    return;
  }

  directionXZ_ = info.predictedKnockbackDirXZ;
  directionXZ_.y = 0.0f;
  if (directionXZ_.LengthSquare() < 1e-6f) {
    return;
  }
  directionXZ_.Normalize();

  const Math::Vector3 start = CapsuleStartWorld();
  const float maxLength = MaxPredictCapsuleLengthMeters();
  currentLengthMeters_ = AtomF::ResolveAimCapsuleClampedMaxLength(
      RefOwner(), start, directionXZ_, radiusMeters_, maxLength);
}

} // namespace Atrum
