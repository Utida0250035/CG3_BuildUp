#pragma once

#include "Math/Vector3.h"
#include <cstdint>

namespace Atrum {
class Camera;
} // namespace Atrum

namespace Atrum::Particle::Combat {

/// <summary>
/// 自機が弾を跳ね返した位置でパーティクルを再生する
/// </summary>
void PlayParryReflectEffect(const Math::Vector3 &worldPosition);

/// <summary>
/// 跳ね返した弾がボスに当たった位置でパーティクルを再生する
/// </summary>
void PlayBossHitByReflectedBulletEffect(const Math::Vector3 &worldPosition);

/// <summary>
/// 敵弾が自機に命中した位置でパーティクルを再生する
/// </summary>
void PlayPlayerHitByEnemyBulletEffect(const Math::Vector3 &worldPosition);

/// 自機撃墜演出用のダメージパーティクル（sequenceId でエミッタ名を一意化）
void PlayPlayerDeathDamageScatterEffect(const Math::Vector3 &worldPosition,
                                        uint32_t sequenceId);

/// <summary>
/// ボス撃破演出用の爆発パーティクル（sequenceId でエミッタ名を一意化）
/// </summary>
void PlayBossDeathExplosionEffect(const Math::Vector3 &worldPosition,
                                  uint32_t sequenceId);

void SetEffectCamera(Camera *camera);
void UpdateCombatParticleEffects();
void DrawCombatParticleEffects();
void ClearCombatParticleEffects();

} // namespace Atrum::Particle::Combat
