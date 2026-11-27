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

/// ボス（Hp128・耐久系パッシブなし）。アクティブスキル 3 種と特殊アクション（激昂）を持つ。
struct BossSummonParameters {
  float telegraphSeconds = 1.0f;
  int count = 3;
  int enragedCount = 5;
  float spawnRingRadiusMeters = 2.2f;
};

struct BossShockwaveParameters {
  float telegraphSeconds = 1.1f;
  float expandSeconds = 0.9f;
  float maxRadiusMeters = 7.5f;
  /// 放射状の「筋」の本数（筋と筋の間が安全地帯）。激昂中は enragedSpokeCount。
  int spokeCount = 6;
  int enragedSpokeCount = 8;
  /// 筋の半幅。自機が筋の中で衝撃波の先端に追い越された瞬間だけ被弾する。
  float spokeHalfWidthMeters = 0.8f;
  /// 衝撃波の先端の厚み（半幅）。
  float bandHalfWidthMeters = 0.5f;
};

struct BossDashParameters {
  float telegraphSeconds = 0.9f;
  float speedMetersPerSecond = 6.0f;
  float durationSeconds = 1.1f;
  /// 突進後に動けなくなる秒数。この間は自機弾のダメージが増える。
  float stunSeconds = 2.2f;
};

struct BossParameters {
  int maxHp = 128;
  float bodyRadiusMeters = 1.2f;
  float moveSpeedMetersPerSecond = 0.5f;
  int bulletDamage = 2;
  int explosionDamage = 16;
  float stunDamageMultiplier = 2.0f;
  float appearSeconds = 2.5f;
  float deathSeconds = 3.2f;
  /// 残り Hp 比率がこの値以下になると激昂（特殊アクション）に入る。
  float enrageHpRatio = 0.5f;
  float enrageSeconds = 2.0f;
  float enrageSpeedMultiplier = 1.6f;
  float skillIntervalSeconds = 3.2f;
  float enragedSkillIntervalMultiplier = 0.6f;
  float spawnZMeters = 9.0f;
  /// 雑魚が全滅してからボスが現れるまでの間。
  float appearDelayAfterMinionsSeconds = 1.5f;
  float colorR = 0.35f;
  float colorG = 0.2f;
  float colorB = 0.75f;
  BossSummonParameters summon{};
  BossShockwaveParameters shockwave{};
  BossDashParameters dash{};
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