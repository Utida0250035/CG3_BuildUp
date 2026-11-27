#pragma once

#include "Math/Vector3.h"

#include <cstddef>
#include <memory>

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// 雑魚敵 1 体を生成する（Initialize 済み）。種類は index % variants で決まる。xz のみ使う。
std::unique_ptr<Entity> CreateAtomFEnemyEntity(const Math::Vector3 &xzPosition,
                                               int index);

/// 円周上に count 体の雑魚を並べ、更新ループ後にシーンへ採用させる（更新中に呼んでよい）。
std::size_t SpawnAtomFEnemyWave(int count, float spawnRadiusMeters,
                                float angleOffsetRadians);

/// 指定位置（xz）に雑魚 1 体を出す。更新中に呼んでよい。
void SpawnAtomFEnemyAt(const Math::Vector3 &xzPosition);

} // namespace Atrum::AtomF
