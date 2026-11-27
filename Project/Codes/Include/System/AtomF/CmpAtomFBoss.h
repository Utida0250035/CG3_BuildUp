#pragma once

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/Entity/ComponentUpd.h"

#include <cstdint>

namespace Atrum {

/// atomF のボス（最大 Hp128・耐久系パッシブなし）。
/// 技: 突進（壁に当たると目眩＝特殊アクション）／雑魚召喚／衝撃波。
/// ダメージは自機弾の直撃と雑魚の爆発のみ。出現はステージ進行側が BeginAppear で指示する。
class CmpAtomFBoss final : public ComponentUpd {
public:
  enum class State : std::uint8_t {
    Dormant,        ///< 未出現（見えない・当たらない）
    Appearing,      ///< 出現演出中（無敵）
    Idle,           ///< 自機へ歩く
    TelegraphCharge,
    Charging,
    TelegraphSummon,
    TelegraphNova,
    Stunned,        ///< 突進が壁に当たった後の目眩（被ダメージ増）
    Dying,
  };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

  /// ステージ進行から呼ぶ。Dormant のときだけ有効。
  void BeginAppear();

  /// 自機弾の直撃・雑魚の爆発から呼ぶ。Idle〜Stunned 以外では無視する。
  /// 戻り値: ダメージを受けたか。
  bool TakeDamage(int baseDamage);

  bool IsDormant() const { return state_ == State::Dormant; }
  bool IsDying() const { return state_ == State::Dying; }
  bool IsTargetable() const;
  State GetState() const { return state_; }

  int Health() const { return health_; }
  int MaxHealth() const { return params_.maxHealth; }
  float HealthRatio() const;
  bool IsEnraged() const { return enraged_; }

  /// 現在の状態の進行度 0..1（予備動作の描画用）。
  float StateProgress() const;
  const Math::Vector3 &ChargeDirection() const { return chargeDirection_; }
  /// 召喚の出現位置（ワールド XZ）。i は 0..summonCount-1。
  Math::Vector3 SummonSpawnPosition(int index) const;

private:
  void EnterState(State next);
  void UpdateAppearing(float dt);
  void UpdateIdle(float dt);
  void UpdateTelegraphCharge(float dt);
  void UpdateCharging(float dt);
  void UpdateTelegraphSummon(float dt);
  void UpdateTelegraphNova(float dt);
  void UpdateStunned(float dt);
  void UpdateDying(float dt);

  void ChooseNextSkill();
  void ExecuteSummon();
  void ExecuteNova();
  void BeginDying();
  void CleanUpMinions();
  void ApplyVisual(float dt);
  Math::Vector3 DirectionToPlayerXZ() const;
  bool IsPlayerWithinXZ(float radius) const;
  int CountMinions() const;
  float SkillIntervalSeconds() const;
  float NextRandom01();

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  AtomF::BossParameters params_{};
  State state_ = State::Dormant;
  float stateElapsedSeconds_ = 0.0f;
  float stateDurationSeconds_ = 1.0f;

  int health_ = 128;
  bool enraged_ = false;
  float hurtFlashSeconds_ = 0.0f;
  float sweatCooldownSeconds_ = 0.0f;
  float deathBurstCooldownSeconds_ = 0.0f;

  int skillCycle_ = 0;
  Math::Vector3 chargeDirection_{0.0f, 0.0f, 1.0f};
  bool chargeHitPlayer_ = false;
  float summonAngleOffsetRadians_ = 0.0f;
  std::uint32_t rngState_ = 0x9E3779B9u;
};

} // namespace Atrum
