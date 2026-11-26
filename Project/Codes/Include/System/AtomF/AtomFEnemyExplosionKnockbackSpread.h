#pragma once

#include "Math/Vector3.h"

namespace Atrum {
class CmpHitSphere;
class Entity;
}

namespace Atrum::AtomF {

/// 爆心地を中心とする半径 explosionRadiusMeters の球と、target の各 HitSphere が交差するか。
bool IsExplosionSphereIntersectingTarget(const Math::Vector3 &centerWorld,
                                         float explosionRadiusMeters,
                                         const Entity &target,
                                         const CmpHitSphere &targetHitSphere);

/// 爆心地から半径c 以内の敵へ速さb で吹き飛ばす（連鎖爆発の契機）。
void SpreadEnemyExplosionKnockbackFromCenter(const Entity &explodingEnemy,
                                             const Math::Vector3 &centerWorld);

} // namespace Atrum::AtomF