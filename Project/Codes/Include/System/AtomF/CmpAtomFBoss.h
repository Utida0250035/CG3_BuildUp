#pragma once

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/Entity/ComponentUpd.h"

namespace Atrum {

/// atomF ボス。Hp128・耐久系パッシブなし。
/// アクティブスキル: 召喚 / 放射衝撃波 / 突進。特殊アクション: Hp が半分を切ったときの激昂（一度だけ）。
/// 数値は parameters.json の boss から読む。
class CmpAtomFBoss final : public ComponentUpd {
public:
  enum class State {
    Appearing,
    Chasing,
    TelegraphSummon,
    TelegraphShockwave,
    Shockwave,
    TelegraphDash,
    Dashing,
    Stunned,
    Enraging,
    Dying
  };

  ~CmpAtomFBoss() override;

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;
  void Finalize() override;

  /// 現在のボス（シーンに 1 体）。HUD 描画用。いなければ nullptr。
  static const CmpAtomFBoss *Active();

  int Hp() const { return hp_; }
  int MaxHp() const { return params_.maxHp; }
  float HpRatio() const {
    return params_.maxHp > 0
               ? static_cast<float>(hp_) / static_cast<float>(params_.maxHp)
               : 0.0f;
  }
  float EnrageHpRatio() const { return params_.enrageHpRatio; }
  bool IsEnraged() const { return enraged_; }
  State CurrentState() const { return state_; }

  /// 出現・激昂・死亡中はダメージを受けない。
  bool CanBeDamaged() const;

  void ApplyBulletHit();
  void ApplyExplosionHit();

private:
  void ApplyDamage(int amount);
  void SetState(State next);
  void SetHitEnabled(bool enabled);

  void UpdateAppearing(float dt);
  void UpdateChasing(float dt);
  void UpdateTelegraphSummon(float dt);
  void UpdateTelegraphShockwave(float dt);
  void UpdateShockwave(float dt);
  void UpdateTelegraphDash(float dt);
  void UpdateDashing(float dt);
  void UpdateStunned(float dt);
  void UpdateEnraging(float dt);
  void UpdateDying(float dt);

  void SpawnMinions();
  void BlastAwayMinions(float radiusMeters);
  void EmitEffectAt(const char *fileName, const Math::Vector3 &offset);
  void EmitSpokeEffects(const char *fileName, float distanceMeters);
  int ActiveSpokeCount() const;
  float SpokeAngleRadians(int spokeIndex) const;
  bool IsPlayerInsideSpokes(const Math::Vector3 &playerPos,
                            float frontRadiusMeters) const;
  bool TryGetPlayerPosition(Math::Vector3 &out) const;
  void ApplyVisuals(float dt);

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  AtomF::BossParameters params_{};
  State state_ = State::Appearing;
  float stateElapsed_ = 0.0f;
  float effectTimer_ = 0.0f;
  int hp_ = 128;
  bool enraged_ = false;
  bool shockwaveHitDone_ = false;
  int skillCycle_ = 0;
  float spokeBaseAngle_ = 0.0f;
  Math::Vector3 dashDirection_{0.0f, 0.0f, -1.0f};
  float hitFlashRemaining_ = 0.0f;
  float visualScale_ = 0.0f;
  int deathBurstCount_ = 0;
  bool initialized_ = false;
};

} // namespace Atrum
