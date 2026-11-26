#pragma once

#include "System/AtomF/AtomFParametersRuntime.h"

namespace Atrum::AtomF {

/// parameters.json を読み込んでいればそこから、未読ならフォールバック定数から半径等を取る。
float ResolveArenaInnerRadiusMeters();
float ResolveArenaWallThicknessMeters();
float ResolveArenaWallHeightMeters();
int ResolveArenaMeshSegmentCount();
float ResolveArenaGroundHeightMeters();

float ResolvePlayerBodyRadiusMeters();
/// parameters の半径b（player.aimCapsule.radiusMeters）
float ResolvePlayerAimCapsuleRadiusMeters();
float ResolvePlayerDamageVignetteSeconds();
int ResolvePlayerDamageSePlayCount();
int ResolvePlayerInitialHealthCount();
/// parameters の半径a（player.bullet.hitRadiusMeters）
float ResolvePlayerBulletRadiusMeters();
float ResolvePlayerBulletSpeedMetersPerSecond();
float ResolvePlayerBulletTrailSeconds();
bool IsPlayerBulletHitShapeSphere();
/// 表示モデル名（player.bullet.modelName / enemy.modelName）。未指定は空。
std::string ResolvePlayerBulletModelName();
std::string ResolveEnemyModelName();
float ResolveEnemyBodyRadiusMeters();
float ResolveEnemyKnockbackSpeedMetersPerSecond();
/// parameters の速さa（enemy.moveSpeedMetersPerSecond）
float ResolveEnemyMoveSpeedMetersPerSecond();
float ResolveEnemyChargeIntervalSeconds();
/// parameters の半径c（enemy.explosion.hitRadiusMeters）
const PlayerInvincibilityParameters &ResolvePlayerInvincibilityParameters();
float ResolveEnemyExplosionHitRadiusMeters();
bool IsEnemyExplosionHitShapeSphere();

const EnemyPanicParameters &ResolveEnemyPanicParameters();
const EnemyPeriodicChargeTimingParameters &
ResolveEnemyPeriodicChargeTimingParameters();
const EnemyChainExplosionParameters &ResolveEnemyChainExplosionParameters();

/// 見下ろし追従カメラ。parameters 未読込時は struct 既定値。
CombatCameraParameters ResolveCombatCameraParameters();

} // namespace Atrum::AtomF