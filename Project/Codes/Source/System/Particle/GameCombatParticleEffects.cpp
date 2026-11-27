#include "System/Particle/GameCombatParticleEffects.h"

#include "Camera/Camera.h"
#include "System/Particle/EmitterManager.h"
#include "Scene/CommandScene.h"

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
inline constexpr const char *kEnemyPanicSweatParticleJson =
    "./Assets/Particles/enemy_panic_sweat.json";

inline constexpr const char *kParryReflectEmitterNamePrefix =
    "combat_parry_reflect_";
inline constexpr const char *kPlayerHitEmitterNamePrefix = "combat_player_hit_";
inline constexpr const char *kBossHitReflectedEmitterNamePrefix =
    "combat_boss_hit_reflected_";
inline constexpr const char *kBossDeathExplosionEmitterNamePrefix =
    "combat_boss_death_explosion_";
inline constexpr const char *kPlayerDeathDamageEmitterNamePrefix =
    "combat_player_death_damage_";
inline constexpr const char *kEnemyPanicSweatEmitterNamePrefix =
    "combat_enemy_panic_sweat_";

Particle::Emitter::EmitterManager *CombatEmitterManager() {
  Atrum::Scene *sceneNow = Atrum::CommandScene::Instance()->GetSceneNow();
  return sceneNow ? sceneNow->GetParticleEmitterManager() : nullptr;
}

void PlayOneShotEffect(const char *emitterNamePrefix, const char *jsonFilePath,
                       const Math::Vector3 &worldPosition,
                       const uint32_t sequenceId) {
  Particle::Emitter::EmitterManager *manager = CombatEmitterManager();
  if (!manager) {
    return;
  }
  const std::string emitterName =
      std::string(emitterNamePrefix) + std::to_string(sequenceId);
  manager->AddOneShotEmitter(emitterName, jsonFilePath, worldPosition);
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

void PlayEnemyPanicSweatEffect(const Math::Vector3 &worldPosition,
                               const uint32_t sequenceId) {
  PlayOneShotEffect(kEnemyPanicSweatEmitterNamePrefix,
                    kEnemyPanicSweatParticleJson, worldPosition, sequenceId);
}

#define ATOMF_DEFINE_ONE_SHOT_EFFECT(FuncName, Prefix, JsonPath)            \
  void FuncName(const Math::Vector3 &worldPosition) {                       \
    static uint32_t sequence = 0;                                           \
    PlayOneShotEffect(Prefix, JsonPath, worldPosition, ++sequence);         \
  }

ATOMF_DEFINE_ONE_SHOT_EFFECT(PlayEnemyMuzzleFlashEffect,
                             "combat_enemy_muzzle_",
                             "./Assets/Particles/enemy_muzzle_flash.json")
ATOMF_DEFINE_ONE_SHOT_EFFECT(PlayEnemySpawnPopEffect, "combat_enemy_spawn_",
                             "./Assets/Particles/enemy_spawn_pop.json")
ATOMF_DEFINE_ONE_SHOT_EFFECT(PlayBossAppearEffect, "combat_boss_appear_",
                             "./Assets/Particles/boss_appear.json")
ATOMF_DEFINE_ONE_SHOT_EFFECT(PlayBossSlamShockwaveEffect,
                             "combat_boss_slam_",
                             "./Assets/Particles/boss_slam_shockwave.json")
ATOMF_DEFINE_ONE_SHOT_EFFECT(PlayBossChargeAuraEffect, "combat_boss_charge_",
                             "./Assets/Particles/boss_charge_aura.json")
ATOMF_DEFINE_ONE_SHOT_EFFECT(PlayBossDashDustEffect, "combat_boss_dust_",
                             "./Assets/Particles/boss_dash_dust.json")
ATOMF_DEFINE_ONE_SHOT_EFFECT(PlayBossWallImpactEffect, "combat_boss_wall_",
                             "./Assets/Particles/boss_wall_impact.json")

#undef ATOMF_DEFINE_ONE_SHOT_EFFECT

void SetEffectCamera(Camera *camera) {
  if (Particle::Emitter::EmitterManager *manager = CombatEmitterManager()) {
    manager->SetCamera(camera);
  }
}

void UpdateCombatParticleEffects() {
  Particle::Emitter::EmitterManager *manager = CombatEmitterManager();
  if (!manager) {
    return;
  }
  manager->Update();
  manager->RemoveInactive();
}

void DrawCombatParticleEffects() {
  if (Particle::Emitter::EmitterManager *manager = CombatEmitterManager()) {
    manager->Draw();
  }
}

void ClearCombatParticleEffects() {
  if (Particle::Emitter::EmitterManager *manager = CombatEmitterManager()) {
    manager->Clear();
  }
}

} // namespace Atrum::Particle::Combat
