#pragma once

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/Entity/ComponentUpd.h"

#include <cstdint>

namespace Atrum {

class CmpHealthHit;
class CmpHitSphere;
class CmpMeshSphere;

/// atomF ボス。Hp は CmpHealthHit に持たせ、被ダメージ・行動（出現→歩行→スキル→死亡）を
/// このコンポーネントが管理する。耐久系パッシブは持たない。
///   アクティブスキル: 輪衝撃 / 突進 / 召喚（予備動作つき）
///   特殊アクション: ダウン（大ダメージで膝突き、被ダメ増）/ 咆哮（体力半分で怒り）
class CmpAtomFBoss final : public ComponentUpd {
public:
  enum class State {
    Appearing,
    Walking,
    Telegraph,
    Executing,
    Staggered,
    Roaring,
    Dying
  };

  enum class Skill { RingBlast, Dash, Summon };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;

  /// 自機弾の直撃・爆発などでダメージを与える。出現中・死亡中は無効で false。
  bool TakeDamage(int amount);

  State GetState() const { return state_; }
  bool IsDying() const { return state_ == State::Dying; }
  bool IsEnraged() const { return enraged_; }
  int GetHealth() const;
  int GetMaxHealth() const { return params_.maxHealth; }

private:
  void SetHitActive(bool active);
  void ApplyVisual(float radiusScale, const Math::Vector3 &shakeOffset,
                   float r, float g, float b, float whiteFlash);
  float NextRandom01();

  void UpdateAppearing(float dt);
  void UpdateWalking(float dt);
  void UpdateTelegraph(float dt);
  void UpdateExecuting(float dt);
  void UpdateStaggered(float dt);
  void UpdateRoaring(float dt);
  void UpdateDying(float dt);

  void BeginTelegraph(Skill skill);
  void BeginExecuting();
  void BeginStagger();
  void BeginRoar();
  void BeginDying();
  void FinishSkillToWalking();
  Skill PickNextSkill();

  void ExecuteRingBlast();
  void ExecuteSummon();
  float CurrentSkillInterval() const;
  float TelegraphSecondsFor(Skill skill) const;
  Math::Vector3 DirectionToPlayerXZ() const;

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  CmpHealthHit *health_ = nullptr;
  CmpHitSphere *hit_ = nullptr;
  CmpMeshSphere *mesh_ = nullptr;

  AtomF::BossParameters params_{};
  State state_ = State::Appearing;
  Skill currentSkill_ = Skill::RingBlast;
  int skillCycleIndex_ = 0;
  bool enraged_ = false;

  float stateElapsedSeconds_ = 0.0f;
  float skillTimerSeconds_ = 0.0f;
  float effectTimerSeconds_ = 0.0f;
  float shakeTimerSeconds_ = 0.0f;
  float hitFlashSeconds_ = 0.0f;
  float executeDurationSeconds_ = 0.0f;
  bool executeActionDone_ = false;
  Math::Vector3 dashDirection_{};

  float staggerAccumulatedDamage_ = 0.0f;
  float staggerCooldownRemainingSeconds_ = 0.0f;

  float dyingExplosionTimerSeconds_ = 0.0f;
  uint32_t rngState_ = 0x2545F491u;
};

} // namespace Atrum
