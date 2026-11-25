#pragma once

#include "Math/Vector3.h"

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

struct EnemyPredictTargetInfo {
  bool targeted = false;
  /// 予測で吹き飛ぶ向き（XZ 単位）。targeted が true のとき有効。
  Math::Vector3 predictedKnockbackDirXZ{0.0f, 0.0f, 1.0f};
};

/// 自機エイム／自機弾の予測カプセルが敵に当たっているかと、吹き飛び予測方向。
bool TryResolveEnemyPredictTarget(const Entity &enemy,
                                  EnemyPredictTargetInfo &outInfo);

} // namespace Atrum::AtomF
