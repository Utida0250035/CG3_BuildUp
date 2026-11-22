#pragma once

#include "Math/Vector3.h"

#include <memory>

namespace Atrum {

class Entity;

namespace AtomF {

/// 自機の子として自機弾を1発出す（所有権は呼び出し側の CmpChildEntitys）。
std::unique_ptr<Entity> CreatePlayerBulletEntity(
    const Math::Vector3 &worldSpawnPosition,
    const Math::Vector3 &aimDirectionXZUnit);

} // namespace AtomF

} // namespace Atrum