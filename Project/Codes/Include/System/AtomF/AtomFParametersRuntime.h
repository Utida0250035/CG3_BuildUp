#pragma once

#include <string>

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

struct PlayerParameters {
  float moveSpeedMetersPerSecond = 0.5f;
  float hitRadiusMeters = 0.25f;
  float shotCooldownSeconds = 1.0f;
  int initialHealthCount = 99;
  PlayerBulletParameters bullet{};
  PlayerAimCapsuleParameters aimCapsule{};
  PlayerDamageFeedbackParameters damageFeedback{};
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

struct EnemyParameters {
  float hitRadiusMeters = 0.25f;
  float moveSpeedMetersPerSecond = 0.3f;
  float chargeIntervalSeconds = 5.0f;
  float knockbackSpeedMetersPerSecond = 1.0f;
  EnemyExplosionParameters explosion{};
  EnemyPanicParameters panic{};
  EnemyPeriodicChargeTimingParameters periodicCharge{};
  EnemyChainExplosionParameters chainExplosion{};
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

  const CombatCameraParameters &Camera() const { return camera_; }

private:
  bool loaded_ = false;
  ArenaParameters arena_{};
  PlayerParameters player_{};
  EnemyParameters enemy_{};
  CombatCameraParameters camera_{};
};

} // namespace Atrum::AtomF