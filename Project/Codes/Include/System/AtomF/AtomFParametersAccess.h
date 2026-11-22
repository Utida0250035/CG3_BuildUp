#pragma once

namespace Atrum::AtomF {

/// parameters.json を読み込んでいればそこから、未読ならフォールバック定数から半径等を取る。
float ResolvePlayerBodyRadiusMeters();
/// parameters の半径b（player.aimCapsule.radiusMeters）
float ResolvePlayerAimCapsuleRadiusMeters();
float ResolvePlayerDamageVignetteSeconds();
/// parameters の半径a（player.bullet.hitRadiusMeters）
float ResolvePlayerBulletRadiusMeters();
float ResolvePlayerBulletSpeedMetersPerSecond();
float ResolvePlayerBulletTrailSeconds();
bool IsPlayerBulletHitShapeSphere();
float ResolveEnemyBodyRadiusMeters();
float ResolveEnemyKnockbackSpeedMetersPerSecond();
/// parameters の速さa（enemy.moveSpeedMetersPerSecond）
float ResolveEnemyMoveSpeedMetersPerSecond();
float ResolveEnemyChargeIntervalSeconds();
/// parameters の半径c（enemy.explosion.hitRadiusMeters）
float ResolveEnemyExplosionHitRadiusMeters();

} // namespace Atrum::AtomF