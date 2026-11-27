#include "System/AtomF/CmpAtomFStageDirector.h"

#include "System/AtomF/AtomFBossSpawn.h"
#include "System/AtomF/AtomFEnemyFactory.h"
#include "System/AtomF/AtomFParametersAccess.h"
#include "System/AtomF/AtomFSceneEntityAdoptQueue.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "System/AtomF/CmpAtomFBoss.h"
#include "System/AtomF/CmpAtomFEnemyChainExplosion.h"
#include "System/DeltaTime.h"
#include "System/Entity/Components/CmpHitSphere.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityTagRegistry.h"
#include "System/Entity/HitJudge.h"

#include <cmath>
#include <cstdint>

namespace Atrum {

namespace {

constexpr float kTwoPi = 6.28318530718f;
/// 出現演出（当たり無効）が終わるまで、生存数を数えない猶予。
constexpr float kSpawnSettleSeconds = 1.0f;
/// 出現位置は自機からこの距離以上離す。
constexpr float kMinSpawnDistanceFromPlayer = 4.0f;
/// 場に居てよい雑魚の上限（ボス戦の増援で溢れさせない）。
constexpr int kMaxAliveEnemies = 9;

float Rand01() {
  static uint32_t state = 0x9E3779B9u;
  state = state * 1664525u + 1013904223u;
  return static_cast<float>((state >> 8) & 0xFFFFu) / 65535.0f;
}

/// 生きている雑魚（ボス・爆散中を除く）の数。当たり判定が有効なものだけ数える。
int CountAliveGrunts() {
  HitJudge *judge = HitJudge::Instance();
  if (judge == nullptr) {
    return 0;
  }
  EntityTagRegistry &tags = EntityTagRegistry::Instance();
  tags.EnsureLoaded();

  int count = 0;
  for (CmpHitSphere *hit : judge->RegisteredHitSpheres()) {
    if (hit == nullptr || !hit->GetIsActive()) {
      continue;
    }
    Entity &owner = hit->RefOwner();
    if (!owner.IsActive() || !tags.EntityHasRole(owner, "enemy") ||
        owner.HasUpdCmp<CmpAtomFBoss>()) {
      continue;
    }
    ++count;
  }
  return count;
}

} // namespace

void CmpAtomFStageDirector::ResolveDependence() {
  frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
}

Math::Vector3 CmpAtomFStageDirector::PickSpawnPoint(const float radiusMeters) const {
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  const Math::Vector3 playerPos =
      (player != nullptr && player->IsActive()) ? player->GetWorldPosition()
                                                : Math::Vector3{};
  Math::Vector3 point{};
  for (int attempt = 0; attempt < 10; ++attempt) {
    const float angle = Rand01() * kTwoPi;
    point = {std::cos(angle) * radiusMeters, 0.0f, std::sin(angle) * radiusMeters};
    const float dx = point.x - playerPos.x;
    const float dz = point.z - playerPos.z;
    if (dx * dx + dz * dz >=
        kMinSpawnDistanceFromPlayer * kMinSpawnDistanceFromPlayer) {
      break;
    }
  }
  return point;
}

void CmpAtomFStageDirector::SpawnWave(const std::size_t waveIndex) {
  const AtomF::StageParameters &stage =
      AtomF::ParametersRuntime::Instance().Stage();
  if (waveIndex >= stage.waves.size()) {
    return;
  }
  // 同じ種類が固まらないよう、ウェーブ内を円周に等間隔＋揺らぎで散らす。
  int total = 0;
  for (const AtomF::StageWaveSpawn &spawn : stage.waves[waveIndex].spawns) {
    total += spawn.count;
  }
  if (total <= 0) {
    return;
  }
  const float baseAngle = Rand01() * kTwoPi;
  int placed = 0;
  AtomF::EnemyVariantParameters fallback{};
  for (const AtomF::StageWaveSpawn &spawn : stage.waves[waveIndex].spawns) {
    const AtomF::EnemyVariantParameters *variant =
        AtomF::FindEnemyVariantByName(spawn.variantName);
    for (int i = 0; i < spawn.count; ++i, ++placed) {
      const float angle = baseAngle + kTwoPi * static_cast<float>(placed) /
                                          static_cast<float>(total) +
                          (Rand01() - 0.5f) * 0.3f;
      Math::Vector3 point{std::cos(angle) * stage.spawnRadiusMeters, 0.0f,
                          std::sin(angle) * stage.spawnRadiusMeters};
      AtomF::EnqueueSceneEntityAdopt(AtomF::CreateEnemyEntity(
          variant != nullptr ? *variant : fallback, point, ++serial_));
    }
  }
}

void CmpAtomFStageDirector::SpawnReinforcements() {
  const AtomF::BossParameters &boss = AtomF::ParametersRuntime::Instance().Boss();
  const auto &variants = AtomF::ParametersRuntime::Instance().Enemy().variants;
  AtomF::EnemyVariantParameters fallback{};
  const float radius = AtomF::ParametersRuntime::Instance().Stage().spawnRadiusMeters;
  for (int i = 0; i < boss.reinforcementCount; ++i) {
    const AtomF::EnemyVariantParameters *variant = nullptr;
    if (!variants.empty()) {
      variant = &variants[static_cast<std::size_t>(Rand01() * 0.999f *
                                                   static_cast<float>(variants.size()))];
    }
    AtomF::EnqueueSceneEntityAdopt(AtomF::CreateEnemyEntity(
        variant != nullptr ? *variant : fallback, PickSpawnPoint(radius),
        ++serial_));
  }
}

void CmpAtomFStageDirector::SpawnBossNow() {
  // 自機の反対側の広場に登場させる。
  Entity *player = AtomF::ScenePlayerLocator::Instance().PlayerEntity();
  const float arena = AtomF::ResolveArenaInnerRadiusMeters();
  Math::Vector3 away{0.0f, 0.0f, 1.0f};
  if (player != nullptr && player->IsActive()) {
    const Math::Vector3 playerPos = player->GetWorldPosition();
    Math::Vector3 flat{-playerPos.x, 0.0f, -playerPos.z};
    const float lengthSq = flat.LengthSquare();
    if (lengthSq > 1.0f) {
      away = flat * (1.0f / std::sqrt(lengthSq));
    }
  }
  AtomF::SpawnBoss(away * (arena * 0.45f));
  bossSpawned_ = true;
}

void CmpAtomFStageDirector::Update() {
  if (frameDeltaTime_ == nullptr) {
    return;
  }
  const float dt = frameDeltaTime_->GetDeltaTime();
  const AtomF::StageParameters &stage =
      AtomF::ParametersRuntime::Instance().Stage();

  switch (phase_) {
  case Phase::Waves: {
    sinceLastSpawnSeconds_ += dt;
    if (nextWave_ < stage.waves.size()) {
      const AtomF::StageWave &wave = stage.waves[nextWave_];
      bool ready = sinceLastSpawnSeconds_ >= wave.delaySeconds;
      if (!ready && nextWave_ > 0 && wave.advanceWhenAliveAtMost > 0 &&
          sinceLastSpawnSeconds_ >= kSpawnSettleSeconds) {
        ready = CountAliveGrunts() <= wave.advanceWhenAliveAtMost;
      }
      if (ready) {
        SpawnWave(nextWave_);
        ++nextWave_;
        sinceLastSpawnSeconds_ = 0.0f;
      }
    } else if (sinceLastSpawnSeconds_ >= kSpawnSettleSeconds &&
               CountAliveGrunts() == 0) {
      phase_ = Phase::BossWait;
      bossWaitSeconds_ = 0.0f;
    }
    break;
  }
  case Phase::BossWait:
    bossWaitSeconds_ += dt;
    if (bossWaitSeconds_ >= stage.bossAppearDelaySeconds) {
      SpawnBossNow();
      reinforcementTimerSeconds_ = 0.0f;
      phase_ = Phase::BossFight;
    }
    break;
  case Phase::BossFight: {
    const CmpAtomFBoss *boss = CmpAtomFBoss::Current();
    if (boss != nullptr) {
      bossSeen_ = true;
    } else if (bossSeen_) {
      phase_ = Phase::Finished;
      break;
    }
    // 登場演出中・撃破演出中は増援を送らない。
    if (boss != nullptr && boss->IsDamageable()) {
      reinforcementTimerSeconds_ += dt;
      const AtomF::BossParameters &bossParams =
          AtomF::ParametersRuntime::Instance().Boss();
      if (reinforcementTimerSeconds_ >= bossParams.reinforcementIntervalSeconds) {
        reinforcementTimerSeconds_ = 0.0f;
        if (CountAliveGrunts() + bossParams.reinforcementCount <=
            kMaxAliveEnemies) {
          SpawnReinforcements();
        }
      }
    }
    break;
  }
  case Phase::Finished:
    break;
  }
}

} // namespace Atrum
