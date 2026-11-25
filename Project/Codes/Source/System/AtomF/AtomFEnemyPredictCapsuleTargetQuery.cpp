#include "System/AtomF/AtomFEnemyPredictCapsuleTargetQuery.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFPlayerAim.h"
#include "System/AtomF/CmpAtomFPlayerAimCapsule.h"
#include "System/AtomF/CmpAtomFPlayerBulletPredictCapsule.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Entity/HitJudge.h"

#include <algorithm>
#include <cmath>

namespace Atrum::AtomF {

namespace {

float DistancePointToSegmentXZ(const Math::Vector3 &point,
                               const Math::Vector3 &segmentStart,
                               const Math::Vector3 &segmentEnd) {
  Math::Vector3 ab = segmentEnd - segmentStart;
  ab.y = 0.0f;
  Math::Vector3 ap = point - segmentStart;
  ap.y = 0.0f;

  const float lengthSq = ab.LengthSquare();
  if (lengthSq < 1e-8f) {
    return std::sqrt(ap.LengthSquare());
  }

  float t = ap.Dot(ab) / lengthSq;
  t = std::clamp(t, 0.0f, 1.0f);
  const Math::Vector3 closest = segmentStart + ab * t;
  const float dx = point.x - closest.x;
  const float dz = point.z - closest.z;
  return std::sqrt(dx * dx + dz * dz);
}

bool CapsuleHitsEnemy(const Entity &enemy, const Math::Vector3 &start,
                      const Math::Vector3 &end, const float capsuleRadius) {
  const float enemyRadius = ResolveEnemyBodyRadiusMeters();
  const float reach = capsuleRadius + enemyRadius;
  const Math::Vector3 enemyPos = enemy.GetWorldPosition();
  return DistancePointToSegmentXZ(enemyPos, start, end) <= reach;
}

} // namespace

bool TryResolveEnemyPredictTarget(const Entity &enemy,
                                  EnemyPredictTargetInfo &outInfo) {
  outInfo = EnemyPredictTargetInfo{};

  Entity *player = ScenePlayerLocator::Instance().PlayerEntity();
  if (player != nullptr && player->IsActive()) {
    CmpAtomFPlayerAim *aim = player->GetUpdCmp<CmpAtomFPlayerAim>();
    CmpAtomFPlayerAimCapsule *aimCapsule =
        player->GetUpdCmp<CmpAtomFPlayerAimCapsule>();
    if (aim != nullptr && aimCapsule != nullptr && aim->IsAimInputActive() &&
        aimCapsule->CurrentLengthMeters() > 1e-4f) {
      const Math::Vector3 start = aimCapsule->CapsuleStartWorld();
      const Math::Vector3 end = aimCapsule->CapsuleEndWorld();
      if (CapsuleHitsEnemy(enemy, start, end, aimCapsule->RadiusMeters())) {
        outInfo.targeted = true;
        outInfo.predictedKnockbackDirXZ = aim->GetAimDirectionXZ();
        return true;
      }
    }
  }

  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return false;
  }

  for (CmpHitSphere *cmp : judge->RegisteredHitSpheres()) {
    if (cmp == nullptr || !cmp->GetIsActive()) {
      continue;
    }
    Entity &owner = cmp->RefOwner();
    if (!tags.EntityHasRole(owner, "playerWeapon")) {
      continue;
    }
    CmpAtomFPlayerBulletPredictCapsule *bulletCapsule =
        owner.GetUpdCmp<CmpAtomFPlayerBulletPredictCapsule>();
    if (bulletCapsule == nullptr ||
        bulletCapsule->CurrentLengthMeters() <= 1e-4f) {
      continue;
    }
    const Math::Vector3 start = bulletCapsule->CapsuleStartWorld();
    const Math::Vector3 end = bulletCapsule->CapsuleEndWorld();
    if (!CapsuleHitsEnemy(enemy, start, end, bulletCapsule->RadiusMeters())) {
      continue;
    }
    outInfo.targeted = true;
    outInfo.predictedKnockbackDirXZ = bulletCapsule->DirectionXZUnit();
    return true;
  }

  return false;
}

} // namespace Atrum::AtomF
