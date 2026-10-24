#pragma once

#include "System/CmpOrbiter.h"
#include <cstddef>
#include <functional>

namespace Atrum {

class Entity;

namespace OrbiterChildUtils {

size_t CountBossOrbiters(Entity &boss);

void ForEachBossOrbiter(
    Entity &boss,
    const std::function<void(CmpOrbiter &, size_t index, size_t orbiterCount)>
        &callback);

void ForEachBossOrbiter(Entity &boss,
                        const std::function<void(CmpOrbiter &)> &callback);

/// ボス更新を止めた状態で CmpOrbiter の Update のみ実行（カメラ演出中など）
void UpdateBossOrbiterMovementOnly(Entity &boss);

float OrbiterIndexToStartAngleRadians(size_t index, size_t orbiterCount);

} // namespace OrbiterChildUtils

} // namespace Atrum
