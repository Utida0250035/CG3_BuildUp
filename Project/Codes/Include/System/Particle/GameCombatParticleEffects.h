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

/// <summary>
/// 敵の焦りモーション中に天辺から流す汗パーティクル（sequenceId でエミッタ名を一意化）
/// </summary>
void PlayEnemyPanicSweatEffect(const Math::Vector3 &worldPosition,
                               uint32_t sequenceId);

/// atomF の雑魚・ボス演出。いずれもワールド座標で 1 回だけ再生する。
void PlayEnemyMuzzleFlashEffect(const Math::Vector3 &worldPosition);
void PlayEnemySpawnPopEffect(const Math::Vector3 &worldPosition);
void PlayBossAppearEffect(const Math::Vector3 &worldPosition);
void PlayBossSlamShockwaveEffect(const Math::Vector3 &worldPosition);
void PlayBossChargeAuraEffect(const Math::Vector3 &worldPosition);
void PlayBossDashDustEffect(const Math::Vector3 &worldPosition);
void PlayBossWallImpactEffect(const Math::Vector3 &worldPosition);

void SetEffectCamera(Camera *camera);
void UpdateCombatParticleEffects();
void DrawCombatParticleEffects();
void ClearCombatParticleEffects();

} // namespace Atrum::Particle::Combat
