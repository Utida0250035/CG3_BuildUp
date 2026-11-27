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
  float colorR = 0.92f;
  float colorG = 0.28f;
  float colorB = 0.25f;
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
  /// 表示モデル名（Assets/Objects/<名前>/<名前>.obj）。空なら球。
  std::string modelName;
};

/// ボス（Hp128・耐久系パッシブなし）。数値は `parameters.json` の `boss`。
struct BossParameters {
  float hitRadiusMeters = 1.2f;
  int maxHealth = 128;
  /// 自機弾の直撃 / 雑魚の爆発 1 回あたりのダメージ。
  int bulletDamage = 4;
  int explosionDamage = 8;
  /// 壁激突スタン中の被ダメージ倍率。
  float stunDamageMultiplier = 2.0f;
  float moveSpeedMetersPerSecond = 0.5f;
  float appearSeconds = 2.5f;
  float deathSeconds = 3.2f;
  float roarSeconds = 1.6f;
  /// 残り Hp 割合がこれ以下で第 2 形態（咆哮 → スキル強化）。
  float phase2HealthRatio = 0.5f;
  float idleSecondsPhase1 = 3.0f;
  float idleSecondsPhase2 = 1.8f;
  float hitFlashSeconds = 0.12f;
  float stunSeconds = 3.5f;

  int summonCountPhase1 = 3;
  int summonCountPhase2 = 5;
  float summonTelegraphSeconds = 0.9f;
  float summonRingRadiusMeters = 3.0f;

  float shockwaveTelegraphSeconds = 1.1f;
  float shockwaveRadiusPhase1Meters = 4.5f;
  float shockwaveRadiusPhase2Meters = 6.0f;

  float dashTelegraphSeconds = 1.0f;
  float dashSpeedMetersPerSecond = 6.0f;
  float dashMaxSeconds = 1.8f;
  float dashRecoverSeconds = 0.8f;
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

  const CombatCameraParameters &Camera() const { return camera_; }

  const BossParameters &Boss() const { return boss_; }

private:
  bool loaded_ = false;
  ArenaParameters arena_{};
  PlayerParameters player_{};
  EnemyParameters enemy_{};
  CombatCameraParameters camera_{};
  BossParameters boss_{};
};

} // namespace Atrum::AtomF