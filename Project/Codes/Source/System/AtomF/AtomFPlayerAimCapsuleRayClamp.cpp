#include "System/AtomF/AtomFPlayerAimCapsuleRayClamp.h"

#include "System/AtomF/AtomFParametersAccess.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Entity/HitJudge.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace Atrum::AtomF {

namespace {

Math::Vector3 FlattenXZ(const Math::Vector3 &value) {
  return {value.x, 0.0f, value.z};
}

bool TryRayCircleExitLengthXZ(const float startX, const float startZ,
                              const float dirX, const float dirZ,
                              const float circleRadiusMeters,
                              float &outLengthMeters) {
  const float dotPd = startX * dirX + startZ * dirZ;
  const float startSq = startX * startX + startZ * startZ;
  const float radiusSq = circleRadiusMeters * circleRadiusMeters;
  const float discriminant = dotPd * dotPd - startSq + radiusSq;
  if (discriminant < 0.0f) {
    return false;
  }

  const float sqrtDisc = std::sqrt(discriminant);
  const float tHit = -dotPd + sqrtDisc;
  if (tHit <= 0.0f) {
    return false;
  }

  outLengthMeters = tHit;
  return true;
}

bool TryRaySphereEntryLength(const Math::Vector3 &rayStart,
                             const Math::Vector3 &dirUnit,
                             const Math::Vector3 &sphereCenter,
                             const float sphereRadius, float &outLengthMeters) {
  const Math::Vector3 oc = rayStart - sphereCenter;
  const float b = 2.0f * dirUnit.Dot(oc);
  const float c = oc.LengthSquare() - sphereRadius * sphereRadius;
  const float discriminant = b * b - 4.0f * c;
  if (discriminant < 0.0f) {
    return false;
  }

  const float sqrtDisc = std::sqrt(discriminant);
  const float t0 = (-b - sqrtDisc) * 0.5f;
  const float t1 = (-b + sqrtDisc) * 0.5f;
  float tHit = std::numeric_limits<float>::max();
  if (t0 > 1e-4f) {
    tHit = std::min(tHit, t0);
  }
  if (t1 > 1e-4f) {
    tHit = std::min(tHit, t1);
  }
  if (tHit == std::numeric_limits<float>::max()) {
    return false;
  }

  outLengthMeters = tHit;
  return true;
}

} // namespace

bool TryMaxAimCapsuleLengthToArenaWall(const Math::Vector3 &startWorld,
                                       const Math::Vector3 &dirUnitXZ,
                                       const float capsuleRadiusMeters,
                                       float &outMaxLengthMeters) {
  const float wallRadius = ResolveArenaInnerRadiusMeters() -
                           std::max(capsuleRadiusMeters, 0.0f);
  if (wallRadius <= 0.0f) {
    return false;
  }

  const Math::Vector3 dir = FlattenXZ(dirUnitXZ);
  if (dir.LengthSquare() < 1e-6f) {
    return false;
  }
  const Math::Vector3 dirUnit = dir * (1.0f / std::sqrt(dir.LengthSquare()));

  float rayToWallCenter = 0.0f;
  if (!TryRayCircleExitLengthXZ(startWorld.x, startWorld.z, dirUnit.x, dirUnit.z,
                                wallRadius, rayToWallCenter)) {
    return false;
  }

  outMaxLengthMeters = std::max(0.0f, rayToWallCenter - capsuleRadiusMeters);
  return true;
}

bool TryMaxAimCapsuleLengthToEnemies(
    const Entity &player, const Math::Vector3 &startWorld,
    const Math::Vector3 &dirUnitXZ, const float capsuleRadiusMeters,
    const float searchLengthMeters, float &outMaxLengthMeters) {
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  const Math::Vector3 dir = FlattenXZ(dirUnitXZ);
  if (dir.LengthSquare() < 1e-6f) {
    return false;
  }
  const Math::Vector3 dirUnit = dir * (1.0f / std::sqrt(dir.LengthSquare()));

  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return false;
  }

  bool found = false;
  float bestLength = std::numeric_limits<float>::max();

  for (CmpHitSphere *cmp : judge->RegisteredHitSpheres()) {
    if (cmp == nullptr || !cmp->GetIsActive()) {
      continue;
    }
    Entity &owner = cmp->RefOwner();
    if (&owner == &player) {
      continue;
    }
    if (!tags.EntityHasRole(owner, "enemy")) {
      continue;
    }

    const Math::Vector3 ownerPos = owner.GetWorldPosition();
    for (const HitSphere &sphere : cmp->GetSpheres()) {
      const Math::Vector3 center = ownerPos + sphere.localOffset;
      float entryLength = 0.0f;
      if (!TryRaySphereEntryLength(startWorld, dirUnit, center, sphere.radius,
                                   entryLength)) {
        continue;
      }
      if (entryLength > searchLengthMeters) {
        continue;
      }

      const float endLength =
          std::max(0.0f, entryLength - capsuleRadiusMeters);
      if (endLength < bestLength) {
        bestLength = endLength;
        found = true;
      }
    }
  }

  if (!found) {
    return false;
  }

  outMaxLengthMeters = bestLength;
  return true;
}

float ResolveAimCapsuleClampedMaxLength(
    const Entity &player, const Math::Vector3 &startWorld,
    const Math::Vector3 &dirUnitXZ, const float capsuleRadiusMeters,
    const float nominalMaxLengthMeters) {
  float limit = nominalMaxLengthMeters;

  float wallLength = 0.0f;
  if (TryMaxAimCapsuleLengthToArenaWall(startWorld, dirUnitXZ,
                                        capsuleRadiusMeters, wallLength)) {
    limit = std::min(limit, wallLength);
  }

  float enemyLength = 0.0f;
  if (TryMaxAimCapsuleLengthToEnemies(player, startWorld, dirUnitXZ,
                                      capsuleRadiusMeters, nominalMaxLengthMeters,
                                      enemyLength)) {
    limit = std::min(limit, enemyLength);
  }

  return std::max(0.0f, limit);
}

} // namespace Atrum::AtomF