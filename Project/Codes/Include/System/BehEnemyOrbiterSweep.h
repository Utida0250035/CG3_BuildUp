#pragma once

#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpEnemy;
class FrameDeltaTime;

/// オービターを地面付近まで伸ばし、短時間高速回転で薙ぎ払う
class BehEnemyOrbiterSweep : public Behavior {
private:
  enum class Phase {
    Telegraph,
    MoveToSweepStart,
    SpinUnderBoss,
    Sweep,
    /// 集め直線（赤予測線）確定後、移動開始まで待機
    HoldBeforeGather,
    GatherOrbiters
  };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::Telegraph;
  float telegraphTimer_ = 0.0f;
  float moveToStartTimer_ = 0.0f;
  float spinUnderBossTimer_ = 0.0f;
  float sweepTimer_ = 0.0f;
  float holdBeforeGatherTimer_ = 0.0f;
  float gatherOrbitersTimer_ = 0.0f;
  float localOrbitHeightY_ = 0.0f;

  static constexpr float kTelegraphSeconds = 1.0f;
  static constexpr float kMoveToSweepStartSeconds = 0.7f;
  static constexpr float kSpinUnderBossSeconds = 1.5f;
  /// 真下スピン開始半径（大きめにして回転が視認しやすい）
  static constexpr float kSpinUnderBossHubRadius = 3.0f;
  /// 1.5 秒で約 1 周（ゆっくり回して軌道を追いやすくする）
  static constexpr float kSpinUnderBossRotateSpeed =
      2.0f * 3.14159265358979323846f / kSpinUnderBossSeconds;
  static constexpr float kSweepSeconds = 1.15f;
  static constexpr float kHoldBeforeGatherSeconds = 1.0f;
  static constexpr float kGatherOrbitersSeconds = 0.85f;
  /// XZ 平面（横広い）の軌道半径
  static constexpr float kSweepOrbitRadius = 30.0f;
  /// 攻撃時間で約 3/4 周する角速度（以前より大幅に低速）
  static constexpr float kSweepRotateSpeed =
      1.5f * 3.14159265358979323846f / kSweepSeconds;

  void BeginMoveToSweepStartPhase();
  void UpdateMoveToSweepStart();
  void BeginSpinUnderBossPhase();
  void BeginSweepPhase();
  void UpdateSweepRadiusScale();
  void BeginHoldBeforeGatherPhase();
  void BeginGatherOrbitersPhase();
  void UpdateGatherOrbiters();
  void RestoreOrbiters();
  void TransitionToRush();

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
  void Finalize() override;
};

} // namespace Atrum
