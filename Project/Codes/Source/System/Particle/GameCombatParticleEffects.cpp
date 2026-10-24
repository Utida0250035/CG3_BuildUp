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

inline constexpr const char *kParryReflectEmitterName = "combat_parry_reflect";
inline constexpr const char *kPlayerHitEmitterName = "combat_player_hit";
inline constexpr const char *kBossHitReflectedEmitterName =
    "combat_boss_hit_reflected";
inline constexpr const char *kBossDeathExplosionEmitterNamePrefix =
    "combat_boss_death_explosion_";

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
  CombatEmitterManager()->AddOneShotEmitter(
      kParryReflectEmitterName, kParryReflectParticleJson, worldPosition,
      kUseBuiltinSphereDraw);
}

void PlayBossHitByReflectedBulletEffect(const Math::Vector3 &worldPosition) {
  CombatEmitterManager()->AddOneShotEmitter(
      kBossHitReflectedEmitterName, kBossHitReflectedParticleJson,
      worldPosition, kUseBuiltinSphereDraw);
}

void PlayPlayerHitByEnemyBulletEffect(const Math::Vector3 &worldPosition) {
  CombatEmitterManager()->AddOneShotEmitter(
      kPlayerHitEmitterName, kPlayerHitEnemyBulletParticleJson, worldPosition,
      kUseBuiltinSphereDraw);
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
