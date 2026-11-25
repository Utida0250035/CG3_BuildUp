#pragma once

#include "Math/Vector3.h"

#include <memory>

namespace Atrum {

class Entity;

namespace AtomF {

/// 自機とは独立した自機弾エンティティを1発作る（所有権は呼び出し側）。
/// worldSpawnPosition は発射時点の自機ワールド座標。
std::unique_ptr<Entity> CreatePlayerBulletEntity(
    const Math::Vector3 &aimDirectionXZUnit,
    const Math::Vector3 &worldSpawnPosition);

} // namespace AtomF

} // namespace Atrum