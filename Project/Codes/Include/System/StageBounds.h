#pragma once

#include "Geometry/AABB.h"
#include "Math/Vector3.h"

namespace Atrum {

// Transplant の剛体が使っていた横画面の端
inline constexpr float kGroundHeight = -4.0f;
inline constexpr float kCeilHeight = 8.0f;
inline constexpr float kLeftEndX = -14.0f;
inline constexpr float kRightEndX = 14.0f;

/// メテオ／壁弾など画面上部から落とす弾の生成 Y（カメラ視野にも含める）
inline constexpr float kBarrageSpawnTopY = kCeilHeight + 1.25f;

/// 戦闘エリア AABB の Z 範囲（奥行き突進・反射弾の追従を含む）
inline constexpr float kCombatAreaMinZ = -4.0f;
inline constexpr float kCombatAreaMaxZ = 18.0f;

/// ボス戦で弾が存在してよい 3 次元 AABB（XY はステージ端、Y 上端は barrage 生成高さ）
inline constexpr Geometry::AABB kCombatAreaAabb{
    {kLeftEndX, kGroundHeight, kCombatAreaMinZ},
    {kRightEndX, kBarrageSpawnTopY, kCombatAreaMaxZ},
};

inline Geometry::AABB GetCombatAreaAabb() { return kCombatAreaAabb; }

inline bool IsInsideCombatAreaAabb(const Math::Vector3 &position) {
  const Geometry::AABB &aabb = kCombatAreaAabb;
  return position.x >= aabb.min.x && position.x <= aabb.max.x &&
         position.y >= aabb.min.y && position.y <= aabb.max.y &&
         position.z >= aabb.min.z && position.z <= aabb.max.z;
}

inline bool IsOutsideCombatAreaAabb(const Math::Vector3 &position) {
  return !IsInsideCombatAreaAabb(position);
}

} // namespace Atrum
