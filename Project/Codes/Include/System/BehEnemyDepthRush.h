#pragma once

#include "Math/Vector3.h"
#include "System/Entity/Behavior.h"

namespace Atrum {

class CmpEnemy;
class FrameDeltaTime;

/// ボス本体が画面奥へ下がり、オービターを伴って自機方向へ突進する
class BehEnemyDepthRush : public Behavior {
private:
  enum class Phase {
    Telegraph,
    RetreatToDepth,
    ChargeTelegraph,
    ChargeToPlayer,
    ReturnTelegraph,
    ReturnToBattle,
  };

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpEnemy *cmpEnemy_ = nullptr;

  Phase phase_ = Phase::Telegraph;
  float telegraphTimer_ = 0.0f;
  float retreatTimer_ = 0.0f;
  float chargeTelegraphTimer_ = 0.0f;
  float chargeTimer_ = 0.0f;
  float returnTelegraphTimer_ = 0.0f;
  float returnTimer_ = 0.0f;

  Math::Vector3 battlePosition_{};
  Math::Vector3 depthPosition_{};
  Math::Vector3 chargeStartPosition_{};
  Math::Vector3 returnMoveStartPosition_{};
  Math::Vector3 lockedPlayerChargeWorld_{};

  static constexpr float kTelegraphSeconds = 0.8f;
  static constexpr float kRetreatToDepthSeconds = 0.7f;
  static constexpr float kChargeTelegraphSeconds = 0.85f;
  static constexpr float kChargeSeconds = 0.65f;
  static constexpr float kReturnTelegraphSeconds = 1.0f;
  static constexpr float kReturnToBattleSeconds = 0.75f;
  /// カメラ奥方向（+Z）へ下がる量
  static constexpr float kDepthBackOffsetZ = 14.0f;

  void LockPlayerChargeWorld();
  void RegisterChargeTelegraph();
  void RegisterReturnTelegraph();
  void PauseOrbitersAtDefaults();
  void RestoreOrbiters();
  void ApplyBossWorldPosition(const Math::Vector3 &worldPosition);

public:
  void ResolveDependence() override;
  void Initialize() override;
  void Execute() override;
  void Finalize() override;
};

} // namespace Atrum
