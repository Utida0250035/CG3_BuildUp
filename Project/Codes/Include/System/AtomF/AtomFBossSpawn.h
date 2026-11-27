#pragma once

#include "Math/Vector3.h"

#include <memory>

namespace Atrum {
class Entity;
}

namespace Atrum::AtomF {

/// ボスを生成して返す（採用は呼び出し側）。役割タグは enemy と boss の両方。
std::unique_ptr<Entity> CreateBossEntity(const Math::Vector3 &spawnPositionXZ);

} // namespace Atrum::AtomF
