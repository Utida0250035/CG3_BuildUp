#pragma once

namespace Atrum::AtomF {

/// parameters.json を読み込んでいればそこから、未読ならフォールバック定数から半径等を取る。
float ResolvePlayerBodyRadiusMeters();
float ResolvePlayerBulletRadiusMeters();
float ResolvePlayerBulletSpeedMetersPerSecond();
float ResolveEnemyBodyRadiusMeters();
float ResolveEnemyKnockbackSpeedMetersPerSecond();

} // namespace Atrum::AtomF