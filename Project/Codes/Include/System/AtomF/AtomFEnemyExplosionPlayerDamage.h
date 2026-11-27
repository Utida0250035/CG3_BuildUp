#pragma once

#include "Math/Vector3.h"

namespace Atrum::AtomF {

/// 爆心地から半径c の爆発球が自機の当たりに触れていれば、自機 HP を 1 減らす（無敵中は無効）。
void ApplyEnemyExplosionDamageToPlayer(const Math::Vector3 &centerWorld);

/// 同様にボスへ。爆発球がボスの当たりに触れていれば boss.explosionDamage を与える。
void ApplyEnemyExplosionDamageToBoss(const Math::Vector3 &centerWorld);

} // namespace Atrum::AtomF
