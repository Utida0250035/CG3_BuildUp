#pragma once

#include "Math/Vector3.h"

namespace Atrum::AtomF {

/// ボスの着地衝撃波（床を広がる輪）を生成する。輪の帯に触れた自機は 1 回だけダメージ。
/// 更新ループ中に呼んでよい（シーンへの採用は走査後）。
void SpawnBossShockwave(const Math::Vector3 &centerWorld, float maxRadiusMeters,
                        float speedMetersPerSecond, float bandHalfWidthMeters);

} // namespace Atrum::AtomF
