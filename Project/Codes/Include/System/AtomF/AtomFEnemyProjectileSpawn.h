#pragma once

#include "Math/Vector3.h"

namespace Atrum::AtomF {

/// 敵弾（ENEMY_WEAPON）を worldPosition から directionXZ へ撃つ。更新中でも安全（採用待ち行列経由）。
/// 色は RGB（0〜1）。速さ・半径・寿命は parameters.json の enemy.projectile。
/// speedScale は弾速の倍率。
void SpawnEnemyProjectile(const Math::Vector3 &worldPosition,
                          const Math::Vector3 &directionXZ, float colorR,
                          float colorG, float colorB, float speedScale = 1.0f);

} // namespace Atrum::AtomF
