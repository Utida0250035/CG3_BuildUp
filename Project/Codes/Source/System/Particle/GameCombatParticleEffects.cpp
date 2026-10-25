#include "System/Particle/GameCombatParticleEffects.h"

#include "Camera/Camera.h"
#include "System/Particle/EmitterManager.h"

#include <string>

namespace Atrum::Particle::Combat {

namespace {

inline constexpr const char *kParryReflectParticleJson =
    "./Assets/Particles/parry_reflect.json";
inline constexpr const char *kBossHitReflectedParticleJson =
    "./Assets/Particles/boss_hit_reflected.json";
inline constexpr const char *kPlayerHitEnemyBulletParticleJson =
    "./Assets/Particles/player_hit_enemy_bullet.json";
inline constexpr const char *kBossDeathExplosionParticleJson =
    "./Assets/Particles/boss_death_explosion.json";

inline constexpr const char *kParryReflectEmitterNamePrefix =
    "combat_parry_reflect_";
inline constexpr const char *kPlayerHitEmitterNamePrefix = "combat_player_hit_";
inline constexpr const char *kBossHitReflectedEmitterNamePrefix =
    "combat_boss_hit_reflected_";
inline constexpr const char *kBossDeathExplosionEmitterNamePrefix =
    "combat_boss_death_explosion_";
inline constexpr const char *kPlayerDeathDamageEmitterNamePrefix =
    "combat_player_death_damage_";

/// 戦闘演出は OBJ 再読込を避け、DrawSphere で描画する
inline constexpr bool kUseBuiltinSphereDraw = true;

Particle::Emitter::EmitterManager *CombatEmitterManager() {
  return Particle::Emitter::EmitterManager::Instance();
}

void PlayOneShotEffect(const char *emitterNamePrefix, const char *jsonFilePath,
                       const Math::Vector3 &worldPosition,
                       const uint32_t sequenceId) {
  const std::string emitterName =
      std::string(emitterNamePrefix) + std::to_string(sequenceId);
  CombatEmitterManager()->AddOneShotEmitter(emitterName, jsonFilePath,
                                            worldPosition,
                                            kUseBuiltinSphereDraw);
}

} // namespace

void PlayParryReflectEffect(const Math::Vector3 &worldPosition) {
  static uint32_t sequence = 0;
  PlayOneShotEffect(kParryReflectEmitterNamePrefix, kParryReflectParticleJson,
                    worldPosition, ++sequence);
}

void PlayBossHitByReflectedBulletEffect(const Math::Vector3 &worldPosition) {
  static uint32_t sequence = 0;
  PlayOneShotEffect(kBossHitReflectedEmitterNamePrefix,
                    kBossHitReflectedParticleJson, worldPosition, ++sequence);
}

void PlayPlayerHitByEnemyBulletEffect(const Math::Vector3 &worldPosition) {
  static uint32_t sequence = 0;
  PlayOneShotEffect(kPlayerHitEmitterNamePrefix,
                    kPlayerHitEnemyBulletParticleJson, worldPosition,
                    ++sequence);
}

void PlayPlayerDeathDamageScatterEffect(const Math::Vector3 &worldPosition,
                                        const uint32_t sequenceId) {
  PlayOneShotEffect(kPlayerDeathDamageEmitterNamePrefix,
                    kPlayerHitEnemyBulletParticleJson, worldPosition,
                    sequenceId);
}

void PlayBossDeathExplosionEffect(const Math::Vector3 &worldPosition,
                                  const uint32_t sequenceId) {
  PlayOneShotEffect(kBossDeathExplosionEmitterNamePrefix,
                    kBossDeathExplosionParticleJson, worldPosition,
                    sequenceId);
}

void SetEffectCamera(Camera *camera) {
  CombatEmitterManager()->SetCamera(camera);
}

void UpdateCombatParticleEffects() {
  Particle::Emitter::EmitterManager *manager = CombatEmitterManager();
  manager->Update();
  manager->RemoveInactive();
}

void DrawCombatParticleEffects() {
  CombatEmitterManager()->Draw();
}

void ClearCombatParticleEffects() {
  CombatEmitterManager()->Clear();
}

} // namespace Atrum::Particle::Combat
