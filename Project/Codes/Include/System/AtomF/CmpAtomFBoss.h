#pragma once

#include "Math/Vector3.h"
#include "System/AtomF/AtomFParametersRuntime.h"
#include "System/Entity/ComponentUpd.h"

#include <vector>

namespace Atrum {

/// atomF のボス。Hp は `parameters.json` の boss.maxHealth（耐久系パッシブなし）。
/// アクティブスキル: 全方位弾 / 狙い撃ち連射 / 雑魚召喚（散開 / 自機を囲む射手 / 両翼の突進型 / 自機の背後を塞ぐ横一列の 4 種）/ スパイラル弾幕 / 薙ぎ払い（自機方向を中心に扇状へ弾列を振る。向きは毎回左右ランダム）/ 十字砲火（8 方向を波ごとに半ピッチずらして撃つ）/ 風車（4 腕で回転し途中で反転する弾幕）/ 扇弾3連（自機狙いの扇状の弾を毎波狙い直して3波）/ 二重螺旋（時計回り・反時計回りの 2 組の腕が交差しながら広がる弾幕）/ 残響（波ごとに速さが上がる全方位リングを 5 連発し、後続が前の波に追いつく）/ 包囲収束（外周の 9 点から自機へ収束する弾を 3 波）/ 隙間リング（一方向だけ弾の無い隙間を空けた全方位弾。狂暴化後は専用の行動順）。近接: 突進→着地衝撃波→硬直 / 跳躍叩きつけ（自機の足元へ跳び、着地点の小さな衝撃波＋硬直）。
/// 自機弾・雑魚の爆発で ApplyDamage される。Hp 0 で撃破演出に入る。
class CmpAtomFBoss final : public ComponentUpd {
public:
  enum class State {
    Entrance,
    Idle,
    Telegraph,
    Execute,
    Dash,
    Leap,
    Recover,
    Dying
  };
  enum class Skill {
    Ring,
    Volley,
    Summon,
    SummonSurround,
    SummonFlank,
    SummonCutoff,
    SummonVanguard,
    SummonGuard,
    Dash,
    Spiral,
    GapRing,
    Sweep,
    Slam,
    Fan,
    Pinwheel,
    Crossfire,
    Converge,
    Helix,
    Echo,
    Rampage,
    Bloom,
    Lattice,
    Pendulum,
    Curtain,
    Scatter,
    Crescent,
    Downpour,
    Pincer,
    Carousel,
    Tide,
    Zigzag,
    Comet,
    Cascade,
    Hourglass,
    Pentacle,
    Snowflake,
    Windmill,
    Rosette,
    Kaleidoscope,
    Whirlpool
  };

  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  ~CmpAtomFBoss() override;

  void ResolveDependence() override;
  void Initialize() override;
  void Update() override;
  void Finalize() override;

  /// シーン上のボス（居なければ nullptr）。
  static CmpAtomFBoss *Current();

  State CurrentState() const { return state_; }
  int Health() const { return health_; }
  int MaxHealth() const { return params_.maxHealth; }
  /// 登場演出中は 0→1 に伸びる Hp バー表示の割合（登場後は 1）。
  float HpBarReveal01() const;
  /// HUD 用: 実際の Hp 割合と、遅れて追従する残像の割合（0〜1）。
  float HealthRatio() const {
    return static_cast<float>(health_) / static_cast<float>(params_.maxHealth);
  }
  float DisplayedHealthRatio() const { return displayedHealthRatio_; }
  /// 撃破演出を含め、まだダメージを受けられる状態か（登場中・撃破中は無敵）。
  bool IsDamageable() const {
    return state_ != State::Entrance && state_ != State::Dying;
  }
  bool IsDying() const { return state_ == State::Dying; }
  bool IsEnraged() const { return enraged_; }

  /// ダメージを与える。受け付けたら true。hitPosition は被弾演出の位置。
  bool ApplyDamage(int baseAmount, const Math::Vector3 &hitPosition);

private:
  void BeginTelegraph(Skill skill);
  void UpdateEntrance(float dt);
  void UpdateIdle(float dt);
  void UpdateTelegraph(float dt);
  void UpdateExecute(float dt);
  void UpdateDash(float dt);
  void UpdateLeap(float dt);
  void UpdateRecover(float dt);
  void UpdateDying(float dt);
  void BeginDying();
  void BeginDash();
  void EndDash();
  void BeginLeap();
  void EndLeap();
  void FinishSkill();
  void ExecuteRing();
  void ExecuteGapRing();
  void ExecuteSummon();
  void FireVolleyShot();
  void FireSpiralShot();
  void FireSweepShot();
  void FireFanWave();
  void FirePinwheelShot();
  void FireHelixShot();
  void FireEchoWave();
  void FireBloomWave();
  void FireLatticeWave();
  void FireCurtainWave();
  void FireScatterBurst();
  void FireCrescentWave();
  void FireDownpourShot();
  void FirePincerShot();
  void FireCarouselShot();
  void FireTideShot();
  void FireZigzagShot();
  void FireCometShot();
  void FireCascadeShot();
  void FireHourglassShot();
  void FirePentacleShot();
  void FireSnowflakeShot();
  void FireWindmillShot();
  void FireRosetteShot();
  void FireKaleidoscopeShot();
  void FireWhirlpoolShot();
  void FirePendulumShot();
  void FireCrossfireWave();
  void FireConvergeWave();
  void MoveToward(const Math::Vector3 &targetXZ, float speed, float dt);
  Math::Vector3 DirectionToPlayerXZ() const;
  void ApplyVisual(float dt);
  void ClearTelegraph();
  void SetDrawJitter(float amplitude, float time);
  void DestroyHostileEntitiesOnDeath();

  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  AtomF::BossParameters params_{};
  float radius_ = 1.1f;

  State state_ = State::Entrance;
  Skill skill_ = Skill::Ring;
  int health_ = 128;
  bool enraged_ = false;
  float stateElapsed_ = 0.0f;
  float idleTimer_ = 0.0f;
  std::size_t skillCursor_ = 0;
  float hitFlashSeconds_ = 0.0f;
  float displayedHealthRatio_ = 1.0f;

  // スキル進行
  int volleyShotsLeft_ = 0;
  float volleyTimer_ = 0.0f;
  int spiralShotsLeft_ = 0;
  float gapAngle_ = 0.0f;
  float spiralAngle_ = 0.0f;
  int sweepShotsLeft_ = 0;
  int fanWavesLeft_ = 0;
  /// 連続突進の残り回数。
  int rampageDashesLeft_ = 0;
  float sweepStartAngle_ = 0.0f;
  float sweepDirection_ = 1.0f;
  Math::Vector3 slamTarget_{};
  float carouselBaseAngle_ = 0.0f;
  Math::Vector3 leapStart_{};
  float recoverOverrideSeconds_ = -1.0f;
  Math::Vector3 lockedDirection_{0.0f, 0.0f, 1.0f};
  std::vector<Math::Vector3> summonPoints_;

  // 演出
  bool entranceEffectPlayed_ = false;
  bool barGlowPlayed_ = false;
  float deathEffectTimer_ = 0.0f;
  int deathBurstCount_ = 0;
  Math::Vector3 deathAnchor_{};
};

} // namespace Atrum
