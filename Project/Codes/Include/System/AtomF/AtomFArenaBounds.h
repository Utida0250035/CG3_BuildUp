#pragma once

#include "Math/Vector3.h"

#include <cmath>

namespace Atrum::AtomF {

/// アリーナ内径。床・壁と同じ 16m（parameters.json のアリーナ記述と一致）。
inline constexpr float kArenaInnerRadiusMeters = 16.0f;

/// parameters.json の半径a（自機弾）
inline constexpr float kPlayerBulletBodyRadiusMeters = 0.125f;

/// parameters.json の半径b（自機本体・エイム）
inline constexpr float kPlayerBodyRadiusMeters = 0.25f;

/// 敵本体の押し出し半径（爆発半径c とは別。当たり未実装のため半径b と同じ）。
inline constexpr float kEnemyBodyRadiusMeters = 0.25f;

/// ワールド XZ を円形アリーナ内に収める。bodyRadius は当たり半径の余白。
inline void ClampWorldTranslateToArenaDisc(Math::Vector3 &worldTranslate,
                                           const float bodyRadiusMeters) {
  const float maxRadius = kArenaInnerRadiusMeters - bodyRadiusMeters;
  if (maxRadius <= 0.0f) {
    worldTranslate.x = 0.0f;
    worldTranslate.z = 0.0f;
    return;
  }

  const float x = worldTranslate.x;
  const float z = worldTranslate.z;
  const float distSq = x * x + z * z;
  const float maxSq = maxRadius * maxRadius;
  if (distSq <= maxSq) {
    return;
  }

  const float dist = std::sqrt(distSq);
  if (dist <= 0.0f) {
    return;
  }

  const float scale = maxRadius / dist;
  worldTranslate.x = x * scale;
  worldTranslate.z = z * scale;
}

} // namespace Atrum::AtomF