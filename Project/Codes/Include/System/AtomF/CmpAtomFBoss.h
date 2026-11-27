#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/Entity/ComponentUpd.h"

#include <cstdint>
#include <vector>

namespace Atrum {

class CmpHitSphere;
class CmpMeshSphere;
class FrameDeltaTime;

/// atomF ボス。最大 Hp128・耐久系パッシブ無し。
///   アクティブスキル: 全方位弾幕 / 子分の召喚 / 叩きつけ（予告円 → 跳躍 → 衝撃波）
///   特殊アクション: 壁まで突進 → 衝突して気絶（被ダメージ増）。第 2 形態（Hp 半分以下）で苛烈化。
/// 与ダメージ: 自機弾（TakeDamage）と、敵の爆発（AtomFEnemyExplosionKnockbackSpread から TakeDamage）。
class CmpAtomFBoss final : public ComponentUpd {
public:
  enum class State {
    Appear,
    Idle,
    Enrage,
    RingTelegraph,
    RingFire,
    SummonTelegraph,
    SlamTelegraph,
    SlamLeap,
    SlamRecover,
    DashTelegraph,
    Dashing,
    Stunned,
    Dying,
  };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;
  void Finalize() override;

  /// いまシーン上に居るボス（居なければ nullptr）。
  static CmpAtomFBoss *Active();

  enum class DamageSource { PlayerBullet, Explosion };

  /// 被弾。出現中・死亡中は無効。受理したら true。
  bool TakeDamage(DamageSource source);

  State CurrentState() const { return state_; }
  int Health() const { return health_; }
  int MaxHealth() const { return params_.maxHealth; }
  bool IsDying() const { return state_ == State::Dying; }
  bool IsStunned() const { return state_ == State::Stunned; }
  bool IsPhase2() const { return phase2_; }

  // --- 予備動作の描画用（CmpDrawAtomFBossTelegraph が読む） ---
  /// 現在の予備動作の進行度 0〜1（予備動作中でなければ 0）。
  float TelegraphProgress() const;
  const Math::Vector3 &SlamTarget() const { return slamTarget_; }
  float SlamRadiusMeters() const { return params_.slamRadiusMeters; }
  const Math::Vector3 &DashDirection() const { return dashDirection_; }
  const std::vector<Math::Vector3> &SummonPoints() const {
    return summonPoints_;
  }
  int CurrentRingBulletCount() const;

private:
  void EnterState(State next);
  void UpdateAppear(float dt);
  void UpdateIdle(float dt);
  void UpdateEnrage(float dt);
  void UpdateRing(float dt);
  void UpdateSummon(float dt);
  void UpdateSlam(float dt);
  void UpdateDash(float dt);
  void UpdateStunned(float dt);
  void UpdateDying(float dt);
  void BeginDeath();
  void ChooseNextSkill();
  void FireRingBurst();
  void ExecuteSummon();
  void LandSlam();
  void ChaseTowardPlayer(float dt, float stopDistance);
  void PlayAuraIfDue(float dt, float interval);
  void ApplyVisual();
  void SetHitEnabled(bool enabled);
  Math::Vector3 PlayerPosition(bool &hasPlayer) const;
  int CountMinions();
  float NextRandom01();

  FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpHitSphere *hitSphere_ = nullptr;
  CmpMeshSphere *mesh_ = nullptr;
  AtomF::BossParameters params_{};

  State state_ = State::Appear;
  float stateTime_ = 0.0f;
  float skillTimer_ = 0.0f;
  float auraTimer_ = 0.0f;
  float hitFlashTimer_ = 0.0f;
  int health_ = 128;
  int skillCount_ = 0;
  int lastSkill_ = -1;
  bool phase2_ = false;
  bool phase2Announced_ = false;
  bool appearFxPlayed_ = false;
  bool landFxPlayed_ = false;
  bool deathFinalBurstPlayed_ = false;
  bool destroyed_ = false;

  int burstsFired_ = 0;
  float burstTimer_ = 0.0f;

  Math::Vector3 slamStart_{};
  Math::Vector3 slamTarget_{};
  Math::Vector3 dashDirection_{0.0f, 0.0f, 1.0f};
  float dashDustTimer_ = 0.0f;
  float deathBurstTimer_ = 0.0f;
  std::vector<Math::Vector3> summonPoints_{};

  uint32_t rngState_ = 0x9E3779B9u;
  float bodyRadius_ = 1.2f;
  Math::Vector4 baseColor_{};
};

} // namespace Atrum
