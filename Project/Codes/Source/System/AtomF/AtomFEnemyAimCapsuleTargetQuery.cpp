#include "System/AtomF/AtomFEnemyAimCapsuleTargetQuery.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/CmpAtomFPlayerAim.h"
#include "System/AtomF/CmpAtomFPlayerAimCapsule.h"
#include "System/Entity/Entity.h"

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

} // namespace

bool IsEnemyTargetedByPlayerAimCapsule(const Entity &enemy, Entity &player) {
  const CmpAtomFPlayerAim *aim = player.GetUpdCmp<CmpAtomFPlayerAim>();
  CmpAtomFPlayerAimCapsule *capsule =
      player.GetUpdCmp<CmpAtomFPlayerAimCapsule>();
  if (aim == nullptr || capsule == nullptr) {
    return false;
  }
  if (!aim->IsAimInputActive() || capsule->CurrentLengthMeters() <= 1e-4f) {
    return false;
  }

  const Math::Vector3 start = capsule->CapsuleStartWorld();
  const Math::Vector3 end = capsule->CapsuleEndWorld();
  const float capsuleRadius = capsule->RadiusMeters();
  const float enemyRadius = ResolveEnemyBodyRadiusMeters();
  const float reach = capsuleRadius + enemyRadius;

  const Math::Vector3 enemyPos = enemy.GetWorldPosition();
  const float dist = DistancePointToSegmentXZ(enemyPos, start, end);
  return dist <= reach;
}

} // namespace Atrum::AtomF