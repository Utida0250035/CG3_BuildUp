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

/// 弾を撃つ前の予測線が出た際に警告音（seBossBulletWarn.mp3）を鳴らす
void PlayBulletWarningSound();

/// 赤色（反射不可）の予測線表示開始時（seBossMeleeWarn.mp3）
void PlayBossMeleeWarnSound();

/// 赤予測線の表示区間を区切る（フェーズ開始時に呼ぶ）
void BeginBossMeleeTelegraphSession(CmpAttackTelegraph *telegraph);

/// ボス赤予測線を登録（同一セッション内の初回登録で seBossMeleeWarn.mp3）
void AddBossMeleeTelegraphLine(CmpAttackTelegraph *telegraph,
                               const Math::Vector3 &start,
                               const Math::Vector3 &end);

bool IsBossMeleeTelegraphColor(const Math::Vector4 &color);

/// シーン退出・再戦時に近接警告 SE 用の静的状態をクリアする
void ResetBossMeleeTelegraphWarnSessions();

} // namespace Atrum
