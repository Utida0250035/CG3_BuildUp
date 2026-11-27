#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Atrum::AtomF {

/// 敵弾を 1 発、更新ループ後にシーンへ採用される形で生成する（更新中に呼んでよい）。
/// direction は XZ 平面の向き（正規化不要）。position は発射位置（ワールド）。
void SpawnEnemyProjectile(const Math::Vector3 &position,
                          const Math::Vector3 &directionXZ,
                          float speedMetersPerSecond, float radiusMeters,
                          const Math::Vector4 &color);

} // namespace Atrum::AtomF
