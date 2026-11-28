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
  /// >0 で自機の周りを回りながら近づく（周回半径。時間とともに orbitShrinkPerSecond で縮む）。
  float orbitRadiusMeters = 0.0f;
  float orbitShrinkPerSecond = 0.25f;
  float orbitMinRadiusMeters = 0.8f;
  /// >0 でこの距離まで近づいたら立ち止まる（射手用）。
  float keepDistanceMeters = 0.0f;
  /// >0 でこの距離より自機に近づかれたら後ずさって間合いを保つ（撃ち下がり用）。
  float retreatDistanceMeters = 0.0f;
  /// >0 で射手になる。予備動作（震え＋照準線）の後、自機へ向けて弾を撃つ間隔。
  float shotIntervalSeconds = 0.0f;
  float shotTelegraphSeconds = 0.7f;
  float shotSpeedMetersPerSecond = 2.4f;
  float shotRadiusMeters = 0.14f;
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

/// ボス（Hp 固定・耐久系パッシブなし）。数値はすべて `parameters.json` の `boss`。
struct BossParameters {
  int maxHealth = 128;
  float hitRadiusMeters = 1.1f;
  float moveSpeedMetersPerSecond = 0.55f;
  /// 自機とこの距離（中心間）以内では前進せず待機する。
  float holdDistanceMeters = 3.0f;
  float colorR = 0.55f;
  float colorG = 0.12f;
  float colorB = 0.75f;
  /// 自機弾 1 発・敵爆発 1 回あたりのダメージ。
  int bulletDamage = 2;
  int explosionDamage = 8;
  /// 突進後の硬直中に受けるダメージ倍率。
  float recoverDamageMultiplier = 1.5f;
  float entranceSeconds = 2.6f;
  float deathSeconds = 3.2f;
  float skillCooldownSeconds = 2.0f;
  /// Hp 割合がこれ以下で行動間隔が enrageCooldownMultiplier 倍になる（耐久ではなく攻撃の激化）。
  float enrageHealthRatio = 0.4f;
  float enrageCooldownMultiplier = 0.6f;

  // スキル: 全方位弾
  int ringBulletCount = 18;
  float ringTelegraphSeconds = 1.0f;
  float ringBulletSpeedMetersPerSecond = 2.0f;
  float ringBulletRadiusMeters = 0.17f;
  // スキル: 狙い撃ち連射
  int volleyShotCount = 6;
  float volleyTelegraphSeconds = 0.9f;
  float volleyIntervalSeconds = 0.2f;
  float volleyBulletSpeedMetersPerSecond = 3.2f;
  // スキル: 雑魚召喚
  int summonCount = 3;
  float summonTelegraphSeconds = 1.1f;
  std::string summonVariantA = "runner";
  std::string summonVariantB = "weaver";
  // 特殊アクション: 突進→着地衝撃波→硬直
  float dashTelegraphSeconds = 1.2f;
  float dashSpeedMetersPerSecond = 8.0f;
  float dashMaxSeconds = 1.0f;
  float recoverSeconds = 2.0f;
  float shockwaveMaxRadiusMeters = 6.0f;
  float shockwaveSpeedMetersPerSecond = 5.0f;
  float shockwaveBandHalfWidthMeters = 0.3f;
  // 戦闘中の増援（雑魚は爆発で本体にダメージを与える弾になる）
  float reinforcementIntervalSeconds = 8.0f;
  int reinforcementCount = 2;
};

struct StageWaveSpawn {
  std::string variantName = "walker";
  int count = 1;
};

struct StageWave {
  /// 前のウェーブの開始（最初は演目開始）からの待ち秒。
  float delaySeconds = 0.0f;
  /// 前ウェーブの生存数がこれ以下になったら（delay 無視で）次を出す。
  int advanceWhenAliveAtMost = 0;
  std::vector<StageWaveSpawn> spawns;
};

struct StageParameters {
  float spawnRadiusMeters = 12.0f;
  float bossAppearDelaySeconds = 2.0f;
  std::vector<StageWave> waves;
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

  const StageParameters &Stage() const { return stage_; }

private:
  bool loaded_ = false;
  ArenaParameters arena_{};
  PlayerParameters player_{};
  EnemyParameters enemy_{};
  CombatCameraParameters camera_{};
  BossParameters boss_{};
  StageParameters stage_{};
};

} // namespace Atrum::AtomF