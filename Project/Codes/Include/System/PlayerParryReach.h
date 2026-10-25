#pragma once

#include "Geometry/AABB.h"
#include "Math/Vector3.h"
#include "System/StageBounds.h"

namespace Atrum {

/// EntityFactory::Parry / CmpDrawPlayerParryField と揃える
inline constexpr float kParryColliderLocalOffsetX = 0.25f;
inline constexpr float kParryColliderLocalOffsetY = 1.0f;
inline constexpr float kParryColliderRadius = 2.0f;
inline constexpr float kManagedEnemyBulletHitRadius = 0.85f;

/// パリィ球と敵弾 Hit が接触しうる余裕（自機移動範囲＋オフセット込み）
inline constexpr float kParryReachPadding =
    kParryColliderRadius + kManagedEnemyBulletHitRadius;

inline Geometry::AABB GetPlayerParryReachAabb() {
  return Geometry::AABB{
      {kLeftEndX + kParryColliderLocalOffsetX - kParryReachPadding,
       kGroundHeight + kParryColliderLocalOffsetY - kParryReachPadding,
       kCombatAreaMinZ},
      {kRightEndX + kParryColliderLocalOffsetX + kParryReachPadding,
       kCeilHeight + kParryColliderLocalOffsetY + kParryReachPadding,
       kCombatAreaMaxZ}};
}

inline bool IsInsidePlayerParryReachAabb(const Math::Vector3 &position) {
  const Geometry::AABB &aabb = GetPlayerParryReachAabb();
  return position.x >= aabb.min.x && position.x <= aabb.max.x &&
         position.y >= aabb.min.y && position.y <= aabb.max.y &&
         position.z >= aabb.min.z && position.z <= aabb.max.z;
}

inline bool IsOutsidePlayerParryReachAabb(const Math::Vector3 &position) {
  return !IsInsidePlayerParryReachAabb(position);
}

} // namespace Atrum
