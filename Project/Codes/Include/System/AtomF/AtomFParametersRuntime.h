#pragma once

#include <string>

namespace Atrum::AtomF {

struct PlayerBulletParameters {
  float speedMetersPerSecond = 1.5f;
  std::string hitShape = "sphere";
  float hitRadiusMeters = 0.125f;
  float trailSeconds = 0.5f;
};

struct PlayerAimCapsuleParameters {
  float radiusMeters = 0.25f;
};

struct PlayerDamageFeedbackParameters {
  float vignetteSeconds = 0.75f;
  int sePlayCount = 1;
};

struct PlayerParameters {
  float moveSpeedMetersPerSecond = 0.5f;
  float hitRadiusMeters = 0.25f;
  float shotCooldownSeconds = 1.0f;
  PlayerBulletParameters bullet{};
  PlayerAimCapsuleParameters aimCapsule{};
  PlayerDamageFeedbackParameters damageFeedback{};
};

struct EnemyExplosionParameters {
  float hitRadiusMeters = 0.4f;
};

struct EnemyParameters {
  float moveSpeedMetersPerSecond = 0.5f;
  float chargeIntervalSeconds = 5.0f;
  float knockbackSpeedMetersPerSecond = 1.5f;
  EnemyExplosionParameters explosion{};
};

/// `Assets/atomF/parameters.json` を読み、自機と敵の数値を保持する（atomF シーン用）。
class ParametersRuntime {
public:
  static ParametersRuntime &Instance();

  bool TryLoadFromJsonFile(const std::string &path);

  void Unload();

  bool IsLoaded() const { return loaded_; }

  const PlayerParameters &Player() const { return player_; }

  const EnemyParameters &Enemy() const { return enemy_; }

private:
  bool loaded_ = false;
  PlayerParameters player_{};
  EnemyParameters enemy_{};
};

} // namespace Atrum::AtomF