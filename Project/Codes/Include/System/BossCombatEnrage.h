#pragma once

#include <cstdint>

namespace Atrum {

class Entity;

namespace BossCombatEnrage {

/// 現在 HP が最大の半分以下か（32 → 16 以下）
bool IsEnraged(Entity &boss);

/// 射撃クールタイム短縮用（1.0 = 通常、大きいほど連射が速い）
float GetShotTempoScale(Entity &boss);

float ScaleShotCoolTimeSeconds(Entity &boss, const float baseCoolTimeSeconds);

uint32_t ScaleShotCountMax(Entity &boss, const uint32_t baseShotCountMax);

/// 公転速度を現在のテンポに合わせて更新
void SyncOrbiterRotateSpeeds(Entity &boss);

} // namespace BossCombatEnrage
} // namespace Atrum
