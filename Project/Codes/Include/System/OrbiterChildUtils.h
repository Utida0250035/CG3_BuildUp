#pragma once

#include "System/Entity/Components/CmpOrbiter.h"
#include <cstddef>
#include <functional>

namespace Atrum {

class Entity;

namespace OrbiterChildUtils {

/// ボス中心からのオービター最大距離（m）
inline constexpr float kBossOrbiterMaxDistanceFromBoss = 2.0f;

size_t CountBossOrbiters(Entity &boss);

void ClampBossOrbiterDistanceFromBoss(
    Entity &boss,
    float maxDistance = kBossOrbiterMaxDistanceFromBoss);

void ForEachBossOrbiterEntity(
    Entity &boss,
    const std::function<void(Entity &orbiterEntity, size_t index,
                             size_t orbiterCount)> &callback);

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
