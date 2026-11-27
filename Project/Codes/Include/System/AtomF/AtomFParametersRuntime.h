#pragma once

#include <string>
#include <vector>

namespace Atrum::AtomF {

struct ArenaParameters {
  float innerRadiusMeters = 16.0f;
  float wallThicknessMeters = 0.4f;
  float wallHeightMeters = 1.5f;
  int meshSegmentCount = 48;
  float groundHeightMeters = 0.0f;
};

struct PlayerBulletParameters {
  float speedMetersPerSecond = 0.75f;
  std::string hitShape = "sphere";
  float hitRadiusMeters = 0.25f;
  float trailSeconds = 0.5f;
  /// 表示モデル名（Assets/Objects/<名前>/<名前>.obj）。空なら球。
  std::string modelName;
};

struct PlayerAimCapsuleParameters {
  float radiusMeters = 0.1f;
};

struct PlayerDamageFeedbackParameters {
  float vignetteSeconds = 0.75f;
  int sePlayCount = 1;
};

/// 被弾後の無敵時間と点滅演出。点滅は無敵開始から pulseSeconds × pulseCount だけ行い、残りは演出なし。
struct PlayerInvincibilityParameters {
  float seconds = 1.0f;
  float blinkPulseSeconds = 0.3f;
  int blinkPulseCount = 3;
  /// 1 周期の終端で到達する最小アルファ（周期の頭で 1.0）。
  float blinkMinAlpha = 0.2f;
};

struct PlayerParameters {
  float moveSpeedMetersPerSecond = 0.5f;
  float hitRadiusMeters = 0.25f;
  float shotCooldownSeconds = 1.0f;
  int initialHealthCount = 99;
  PlayerBulletParameters bullet{};
  PlayerAimCapsuleParameters aimCapsule{};
  PlayerDamageFeedbackParameters damageFeedback{};
  PlayerInvincibilityParameters invincibility{};
};

struct EnemyExplosionParameters {
  std::string hitShape = "sphere";
  float hitRadiusMeters = 0.4f;
};

struct EnemyPanicParameters {
  float lingerAfterAimSeconds = 0.35f;
  float shakeAmplitudeHorizontalMeters = 0.12f;
  float shakeAmplitudeVerticalMeters = 0.045f;
  float wobbleSpeed = 76.0f;
  float sweatEmitIntervalSeconds = 0.14f;
};

struct EnemyPeriodicChargeTimingParameters {
  float shakeDurationSeconds = 0.45f;
  float chargeDurationSeconds = 0.75f;
  float shakeAmplitudeMeters = 0.1f;
  float shakeAngularSpeed = 48.0f;
};

struct EnemyChainExplosionParameters {
  float shrinkDurationSeconds = 0.6f;
};

/// 雑魚敵の種類。全種 Hp1・耐久系能力なし（挙動と見た目だけが違う）。
struct EnemyVariantParameters {
  std::string name = "walker";
  /// enemy.moveSpeedMetersPerSecond への倍率。
  float moveSpeedMultiplier = 1.0f;
  /// 突進間隔。0 以下なら enemy.chargeIntervalSeconds を使い、chargeEnabled が false なら突進しない。
  float chargeIntervalSeconds = 0.0f;
  bool chargeEnabled = true;
  /// 進行方向に垂直な蛇行（0 で直進）。
  float weaveAmplitudeMeters = 0.0f;
  float weaveHz = 0.0f;
  /// 0 より大きいと遠隔射撃型。この間隔ごとに予備動作 → 自機へ弾を撃つ。
  float shootIntervalSeconds = 0.0f;
  /// 射撃型が保つ自機との距離。近すぎると後退し、遠いと近づく（0 で無効）。
  float keepDistanceMeters = 0.0f;
  /// 0 より大きいと周回型。自機とこの距離を保って周りを回る（突進と併用可）。
  float orbitDistanceMeters = 0.0f;
  float colorR = 0.92f;
  float colorG = 0.28f;
  float colorB = 0.25f;
};

/// 敵が撃つ弾（ENEMY_WEAPON）。雑魚の射撃型とボスのスキルが共用する。
struct EnemyProjectileParameters {
  float speedMetersPerSecond = 3.0f;
  float radiusMeters = 0.18f;
  float lifetimeSeconds = 6.0f;
};

/// ボス（最大 Hp128・耐久系パッシブ無し）。スキル・特殊行動の数値。
struct BossParameters {
  int maxHealth = 128;
  float bodyRadiusMeters = 1.2f;
  float moveSpeedMetersPerSecond = 0.35f;
  /// 自機弾 1 発・敵の爆発 1 回が与えるダメージ。
  int playerBulletDamage = 2;
  int explosionDamage = 10;
  /// 気絶中の被ダメージ倍率（弱点を晒す仕様。耐久ではなく脆くなる側）。
  float stunDamageMultiplier = 2.0f;
  /// Hp がこの割合以下で第 2 形態（スキル間隔が短く、弾が増える）。
  float phase2HealthRatio = 0.5f;
  float skillIntervalSeconds = 2.2f;
  float phase2SkillIntervalSeconds = 1.3f;
  float appearSeconds = 2.4f;
  float deathSeconds = 3.2f;
  // スキル 1: 全方位弾幕
  int ringBulletCount = 14;
  int phase2RingBulletCount = 22;
  int ringBurstCount = 3;
  float ringTelegraphSeconds = 1.0f;
  float ringBurstIntervalSeconds = 0.45f;
  // スキル 2: 子分の召喚
  int summonCount = 3;
  int phase2SummonCount = 5;
  float summonTelegraphSeconds = 1.0f;
  // スキル 3: 叩きつけ（予告円 → 衝撃波）
  float slamTelegraphSeconds = 1.2f;
  float slamRadiusMeters = 3.6f;
  float slamRecoverSeconds = 0.6f;
  // 特殊行動: 壁まで突進して気絶（弱点露出）
  float dashTelegraphSeconds = 1.3f;
  float dashSpeedMetersPerSecond = 7.0f;
  float dashStunSeconds = 3.0f;
  /// この回数スキルが続いたら必ず特殊行動（突進）を挟む。
  int dashEveryNSkills = 4;
  float colorR = 0.45f;
  float colorG = 0.12f;
  float colorB = 0.7f;
};

struct EnemyParameters {
  float hitRadiusMeters = 0.25f;
  float moveSpeedMetersPerSecond = 0.3f;
  float chargeIntervalSeconds = 5.0f;
  float knockbackSpeedMetersPerSecond = 1.0f;
  EnemyExplosionParameters explosion{};
  EnemyPanicParameters panic{};
  EnemyPeriodicChargeTimingParameters periodicCharge{};
  EnemyChainExplosionParameters chainExplosion{};
  /// 空なら従来の単一種（walker）のみ。スポーンは index % size で割り当てる。
  std::vector<EnemyVariantParameters> variants;
  EnemyProjectileParameters projectile{};
  /// 表示モデル名（Assets/Objects/<名前>/<名前>.obj）。空なら球。
  std::string modelName;
};

/// 斜め見下ろし追従カメラ（`parameters.json` の `camera`）。
struct CombatCameraFollowEyeOffset {
  float offsetXMeters = 0.0f;
  float offsetYMeters = 11.0f;
  float offsetZMeters = -7.0f;
};

struct CombatCameraFraming {
  float leftEndXMeters = -16.0f;
  float rightEndXMeters = 16.0f;
  float groundHeightMeters = 0.0f;
  float barrageSpawnTopYMeters = 4.0f;
  float pivotZMeters = 0.0f;
};

struct CombatCameraParameters {
  CombatCameraFollowEyeOffset followEyeOffset{};
  CombatCameraFraming framing{};
};

/// `Assets/atomF/parameters.json` を読み、自機と敵の数値を保持する（atomF シーン用）。
class ParametersRuntime {
public:
  static ParametersRuntime &Instance();

  bool TryLoadFromJsonFile(const std::string &path);

  void Unload();

  bool IsLoaded() const { return loaded_; }

  const ArenaParameters &Arena() const { return arena_; }

  const PlayerParameters &Player() const { return player_; }

  const EnemyParameters &Enemy() const { return enemy_; }

  const BossParameters &Boss() const { return boss_; }

  const CombatCameraParameters &Camera() const { return camera_; }

private:
  bool loaded_ = false;
  ArenaParameters arena_{};
  PlayerParameters player_{};
  EnemyParameters enemy_{};
  BossParameters boss_{};
  CombatCameraParameters camera_{};
};

} // namespace Atrum::AtomF