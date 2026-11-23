#include "System/AtomF/AtomFParametersRuntime.h"

#include <nlohmann/json.hpp>
#include <fstream>

namespace Atrum::AtomF {

namespace {

bool TryParsePlayer(const nlohmann::json &json, PlayerParameters &out) {
  if (!json.is_object()) {
    return false;
  }
  if (!json.contains("moveSpeedMetersPerSecond") ||
      !json.contains("hitRadiusMeters") ||
      !json.contains("shotCooldownSeconds") || !json.contains("bullet") ||
      !json.contains("aimCapsule") || !json.contains("damageFeedback")) {
    return false;
  }

  out.moveSpeedMetersPerSecond =
      json.at("moveSpeedMetersPerSecond").get<float>();
  out.hitRadiusMeters = json.at("hitRadiusMeters").get<float>();
  out.shotCooldownSeconds = json.at("shotCooldownSeconds").get<float>();

  const auto &bullet = json.at("bullet");
  if (!bullet.is_object() || !bullet.contains("speedMetersPerSecond") ||
      !bullet.contains("hitShape") || !bullet.contains("hitRadiusMeters") ||
      !bullet.contains("trailSeconds")) {
    return false;
  }
  out.bullet.speedMetersPerSecond =
      bullet.at("speedMetersPerSecond").get<float>();
  out.bullet.hitShape = bullet.at("hitShape").get<std::string>();
  out.bullet.hitRadiusMeters = bullet.at("hitRadiusMeters").get<float>();
  out.bullet.trailSeconds = bullet.at("trailSeconds").get<float>();

  const auto &aim = json.at("aimCapsule");
  if (!aim.is_object() || !aim.contains("radiusMeters")) {
    return false;
  }
  out.aimCapsule.radiusMeters = aim.at("radiusMeters").get<float>();

  const auto &feedback = json.at("damageFeedback");
  if (!feedback.is_object() || !feedback.contains("vignetteSeconds") ||
      !feedback.contains("sePlayCount")) {
    return false;
  }
  out.damageFeedback.vignetteSeconds =
      feedback.at("vignetteSeconds").get<float>();
  out.damageFeedback.sePlayCount = feedback.at("sePlayCount").get<int>();

  return true;
}

bool TryParseEnemy(const nlohmann::json &json, EnemyParameters &out) {
  if (!json.is_object()) {
    return false;
  }
  if (!json.contains("moveSpeedMetersPerSecond") ||
      !json.contains("chargeIntervalSeconds") ||
      !json.contains("knockbackSpeedMetersPerSecond") ||
      !json.contains("explosion")) {
    return false;
  }

  out.moveSpeedMetersPerSecond =
      json.at("moveSpeedMetersPerSecond").get<float>();
  out.chargeIntervalSeconds = json.at("chargeIntervalSeconds").get<float>();
  out.knockbackSpeedMetersPerSecond =
      json.at("knockbackSpeedMetersPerSecond").get<float>();

  const auto &explosion = json.at("explosion");
  if (!explosion.is_object() || !explosion.contains("hitRadiusMeters")) {
    return false;
  }
  if (explosion.contains("hitShape") && explosion.at("hitShape").is_string()) {
    out.explosion.hitShape = explosion.at("hitShape").get<std::string>();
  }
  out.explosion.hitRadiusMeters =
      explosion.at("hitRadiusMeters").get<float>();

  return true;
}

} // namespace

ParametersRuntime &ParametersRuntime::Instance() {
  static ParametersRuntime instance;
  return instance;
}

bool ParametersRuntime::TryLoadFromJsonFile(const std::string &path) {
  loaded_ = false;
  player_ = PlayerParameters{};
  enemy_ = EnemyParameters{};

  std::ifstream stream(path);
  if (!stream) {
    return false;
  }

  nlohmann::json json;
  try {
    stream >> json;
  } catch (...) {
    return false;
  }

  if (!json.is_object() || !json.contains("player") ||
      !json.contains("enemy")) {
    return false;
  }

  PlayerParameters parsedPlayer{};
  EnemyParameters parsedEnemy{};
  if (!TryParsePlayer(json.at("player"), parsedPlayer) ||
      !TryParseEnemy(json.at("enemy"), parsedEnemy)) {
    return false;
  }

  player_ = parsedPlayer;
  enemy_ = parsedEnemy;
  loaded_ = true;
  return true;
}

void ParametersRuntime::Unload() {
  loaded_ = false;
  player_ = PlayerParameters{};
  enemy_ = EnemyParameters{};
}

} // namespace Atrum::AtomF