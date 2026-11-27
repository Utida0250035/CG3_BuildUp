#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF 1 ステージの進行役: parameters.json の stage.waves を順に出し、
/// 全滅（または残りわずか）でボスを登場させ、戦闘中は増援を送る。
/// 敵・ボスの生成は AtomF::CreateEnemyEntity / SpawnBoss に任せ、数値は JSON が持つ。
class CmpAtomFStageDirector final : public ComponentUpd {
public:
  enum class Phase { Waves, BossWait, BossFight, Finished };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  Phase CurrentPhase() const { return phase_; }

private:
  void SpawnWave(std::size_t waveIndex);
  void SpawnBossNow();
  void SpawnReinforcements();
  Math::Vector3 PickSpawnPoint(float radiusMeters) const;

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  Phase phase_ = Phase::Waves;
  std::size_t nextWave_ = 0;
  float sinceLastSpawnSeconds_ = 0.0f;
  float bossWaitSeconds_ = 0.0f;
  float reinforcementTimerSeconds_ = 0.0f;
  bool bossSpawned_ = false;
  bool bossSeen_ = false;
  int serial_ = 0;
};

} // namespace Atrum
