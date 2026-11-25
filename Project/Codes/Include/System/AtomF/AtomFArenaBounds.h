#pragma once

#include "Math/Vector3.h"

namespace Atrum::AtomF {

/// parameters 未読込時のフォールバック（`parameters.json` の `arena` と揃える）。
inline constexpr float kArenaInnerRadiusMeters = 16.0f;
inline constexpr float kArenaWallThicknessMeters = 0.4f;
inline constexpr float kArenaWallHeightMeters = 1.5f;
inline constexpr int kArenaMeshSegmentCount = 48;
inline constexpr float kArenaGroundHeightMeters = 0.0f;

/// game_atomF.md の自機弾半径（0.25m）
inline constexpr float kPlayerBulletBodyRadiusMeters = 0.25f;

/// game_atomF.md の自機弾速さ（0.75m/s）
inline constexpr float kPlayerBulletSpeedMetersPerSecond = 0.75f;

/// parameters.json の軌道残留（自機弾 trailSeconds）
inline constexpr float kPlayerBulletTrailSeconds = 0.5f;

/// 自機本体の押し出し半径。エイム予測カプセルは別（0.1m）。
inline constexpr float kPlayerBodyRadiusMeters = 0.25f;

/// game_atomF.md のエイム予測カプセル半径（0.1m）
inline constexpr float kPlayerAimCapsuleRadiusMeters = 0.1f;

/// 敵本体の押し出し／予測カプセル半径。
inline constexpr float kEnemyBodyRadiusMeters = 0.25f;

/// game_atomF.md の敵吹き飛び速さ（1.0m/s）
inline constexpr float kEnemyKnockbackSpeedMetersPerSecond = 1.0f;

/// parameters.json の半径c（敵爆発の影響）
inline constexpr float kEnemyExplosionHitRadiusMeters = 0.4f;

/// game_atomF.md の敵歩行速さ（0.3m/s）
inline constexpr float kEnemyMoveSpeedMetersPerSecond = 0.3f;

inline constexpr float kEnemyChargeIntervalSeconds = 5.0f;

/// parameters.json の被弾ビネット秒（無敵時間の既定）
inline constexpr float kPlayerDamageVignetteSeconds = 0.75f;

/// ワールド XZ を円形アリーナ内に収める。bodyRadius は当たり半径の余白。
void ClampWorldTranslateToArenaDisc(Math::Vector3 &worldTranslate,
                                    const float bodyRadiusMeters);

/// 円形アリーナの壁に当たり半径分の余白を見て接触しているか。
bool IsWorldXZTouchingArenaWall(const float x, const float z,
                                const float bodyRadiusMeters);

} // namespace Atrum::AtomF