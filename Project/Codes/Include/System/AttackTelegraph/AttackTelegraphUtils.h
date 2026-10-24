#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Atrum {

class CmpAttackTelegraph;
class Entity;

/// 反射不可攻撃など、通常オレンジと区別する予測線色
namespace AttackTelegraphPalette {
inline constexpr Math::Vector4 kNonReflectAbleMain{0.92f, 0.12f, 0.1f, 0.88f};
inline constexpr Math::Vector4 kNonReflectAbleSub{0.85f, 0.08f, 0.06f, 0.5f};
} // namespace AttackTelegraphPalette

/// 上から落とす弾：予測線を見せてから発射するまでの待ち時間（秒）
inline constexpr float kDropAttackTelegraphSeconds = 1.0f;

/// ステージ XY 内でレイを伸ばした終点（z は start に合わせる）
Math::Vector3 ExtendRayWithinStageBounds(const Math::Vector3 &start,
                                         const Math::Vector3 &directionNormalized,
                                         const float maxLength = 48.0f);

void AddVerticalDropTelegraph(class CmpAttackTelegraph *telegraph,
                              const float worldX, const float spawnY,
                              const float groundY);

void RegisterWallShotDropTelegraph(class CmpAttackTelegraph *telegraph,
                                   const float spawnY, const float groundY,
                                   const uint32_t shotCount);

void RegisterFullWidthGroundLines(CmpAttackTelegraph *telegraph, float groundY,
                                  float secondaryLineYOffset,
                                  const Math::Vector4 &mainColor,
                                  const Math::Vector4 &secondaryColor);

void RegisterBossOrbiterGatherTelegraph(Entity &boss,
                                        CmpAttackTelegraph *telegraph,
                                        const Math::Vector4 &lineColor);

} // namespace Atrum
