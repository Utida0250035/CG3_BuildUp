#pragma once

#include "Math/Vector3.h"
#include "System/Entity/ComponentUpd.h"

#include <vector>

namespace Atrum {

/// atomF ボスの行動（出現 → 待機/追尾 → スキル → スタン/咆哮 → 撃破）。
/// Hp は最大 128 の単純な数値。耐久系のパッシブは持たない。
/// 自機弾の直撃と雑魚の爆発でのみダメージを受ける（吹き飛ばない）。
///
/// アクティブスキル: 召喚 / 衝撃波 / 突進。
/// 特殊アクション: 突進が壁に当たると目を回して長時間スタン（被ダメ倍率）、
///                 Hp が半分以下で咆哮して第 2 形態へ。
class CmpAtomFBoss final : public ComponentUpd {
public:
  enum class State {
    Appear,
    Idle,
    SummonTelegraph,
    ShockwaveTelegraph,
    DashTelegraph,
    Dashing,
    DashRecover,
    Stunned,
    Roar,
    Dying,
  };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;
  void Finalize() override;

  /// 現在場にいるボス（居なければ nullptr）。Hp バー描画用。
  static CmpAtomFBoss *Active();

  /// Hp バーの充填率 0..1。出現中は 0 から満タンへ伸びる演出。
  float HealthBar01() const;

  /// 被弾を受け付けたら true。出現・咆哮・撃破中は無効。
  bool TakeDamage(int baseDamage, const Math::Vector3 &hitWorldPosition);

  int Health() const { return health_; }
  int MaxHealth() const { return maxHealth_; }
  State GetState() const { return state_; }
  bool IsDying() const { return state_ == State::Dying; }
  bool IsPhase2() const { return phase2_; }
  bool IsStunned() const { return state_ == State::Stunned; }

  // --- 描画側（CmpDrawAtomFBoss）が読む予兆情報 ---
  /// 予兆が出ている間 true。
  bool HasTelegraph() const;
  /// 予兆の進行 0..1（0=開始, 1=発動）。
  float TelegraphProgress01() const;
  float BodyRadiusMeters() const { return bodyRadiusMeters_; }
  float ShockwaveRadiusMeters() const;
  const std::vector<Math::Vector3> &SummonPoints() const {
    return summonPoints_;
  }
  const Math::Vector3 &DashDirection() const { return dashDirection_; }
  /// 突進予兆の線の長さ（壁までの距離）。
  float DashLineLengthMeters() const { return dashLineLengthMeters_; }
  float DashLineWidthMeters() const { return bodyRadiusMeters_ * 2.0f; }

private:
  enum class Skill { Summon, Shockwave, Dash };

  void EnterState(State next);
  void UpdateAppear(float dt);
  void UpdateIdle(float dt);
  void UpdateSummonTelegraph(float dt);
  void UpdateShockwaveTelegraph(float dt);
  void UpdateDashTelegraph(float dt);
  void UpdateDashing(float dt);
  void UpdateDashRecover(float dt);
  void UpdateStunned(float dt);
  void UpdateRoar(float dt);
  void UpdateDying(float dt);

  void ChooseNextSkill();
  void ExecuteSummon();
  void ExecuteShockwave();
  void BeginDying();
  void VanishAllMinions();
  void DamagePlayerIfInside(float radiusMeters);
  void ApplyVisual(float dt);
  bool IsTouchingArenaWall() const;
  Math::Vector3 DirectionToPlayerXZ(float *outDistance) const;
  float ComputeDashLineLength(const Math::Vector3 &from,
                              const Math::Vector3 &dirXZ) const;

  class FrameDeltaTime *frameDeltaTime_ = nullptr;

  bool started_ = false;
  State state_ = State::Appear;
  float stateElapsedSeconds_ = 0.0f;
  float totalElapsedSeconds_ = 0.0f;
  float idleTimerSeconds_ = 0.0f;
  int skillCursor_ = 0;
  int summonSerial_ = 0;
  bool phase2_ = false;
  bool roarPending_ = false;

  int health_ = 128;
  int maxHealth_ = 128;
  float bodyRadiusMeters_ = 1.2f;
  float hitFlashSeconds_ = 0.0f;
  float dyingExplosionTimerSeconds_ = 0.0f;
  float stunSweatTimerSeconds_ = 0.0f;

  Skill currentSkill_ = Skill::Summon;
  std::vector<Math::Vector3> summonPoints_{};
  Math::Vector3 dashDirection_{0.0f, 0.0f, 1.0f};
  float dashLineLengthMeters_ = 0.0f;
  Math::Vector3 anchorTranslate_{};
};

} // namespace Atrum
