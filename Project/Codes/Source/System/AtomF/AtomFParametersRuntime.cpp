#include "System/AtomF/AtomFParametersRuntime.h"

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::AtomF {

namespace {

bool TryParsePlayer(const nlohmann::json &json, PlayerParameters &out) {
  if (!json.is_object()) {
    return false;
  }
  if (!json.contains("moveSpeedMetersPerSecond") ||
      !json.contains("hitRadiusMeters") ||
      !json.contains("shotCooldownSeconds") ||
      !json.contains("initialHealthCount") || !json.contains("bullet") ||
      !json.contains("aimCapsule") || !json.contains("damageFeedback")) {
    return false;
  }

  out.moveSpeedMetersPerSecond =
      json.at("moveSpeedMetersPerSecond").get<float>();
  out.hitRadiusMeters = json.at("hitRadiusMeters").get<float>();
  out.shotCooldownSeconds = json.at("shotCooldownSeconds").get<float>();
  out.initialHealthCount =
      std::max(1, json.at("initialHealthCount").get<int>());

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
  out.bullet.modelName = bullet.value("modelName", std::string{});

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

  if (json.contains("invincibility") && json.at("invincibility").is_object()) {
    const auto &inv = json.at("invincibility");
    out.invincibility.seconds =
        std::max(0.0f, inv.value("seconds", out.invincibility.seconds));
    out.invincibility.blinkPulseSeconds = std::max(
        0.01f, inv.value("blinkPulseSeconds", out.invincibility.blinkPulseSeconds));
    out.invincibility.blinkPulseCount =
        std::max(0, inv.value("blinkPulseCount", out.invincibility.blinkPulseCount));
    out.invincibility.blinkMinAlpha = std::clamp(
        inv.value("blinkMinAlpha", out.invincibility.blinkMinAlpha), 0.0f, 1.0f);
  }

  return true;
}

void ApplyArenaFromJson(const nlohmann::json &root, ArenaParameters &out) {
  if (!root.contains("arena") || !root.at("arena").is_object()) {
    return;
  }

  const auto &arena = root.at("arena");
  out.innerRadiusMeters =
      arena.value("innerRadiusMeters", out.innerRadiusMeters);
  out.wallThicknessMeters =
      arena.value("wallThicknessMeters", out.wallThicknessMeters);
  out.wallHeightMeters =
      arena.value("wallHeightMeters", out.wallHeightMeters);
  out.meshSegmentCount =
      arena.value("meshSegmentCount", out.meshSegmentCount);
  out.groundHeightMeters =
      arena.value("groundHeightMeters", out.groundHeightMeters);
}

void ApplyCombatCameraFromJson(const nlohmann::json &root,
                               CombatCameraParameters &out) {
  if (!root.contains("camera") || !root.at("camera").is_object()) {
    return;
  }

  const auto &camera = root.at("camera");
  if (camera.contains("followEyeOffset") &&
      camera.at("followEyeOffset").is_object()) {
    const auto &eye = camera.at("followEyeOffset");
    out.followEyeOffset.offsetXMeters =
        eye.value("offsetXMeters", out.followEyeOffset.offsetXMeters);
    out.followEyeOffset.offsetYMeters =
        eye.value("offsetYMeters", out.followEyeOffset.offsetYMeters);
    out.followEyeOffset.offsetZMeters =
        eye.value("offsetZMeters", out.followEyeOffset.offsetZMeters);
  }

  if (camera.contains("framing") && camera.at("framing").is_object()) {
    const auto &framing = camera.at("framing");
    out.framing.leftEndXMeters =
        framing.value("leftEndXMeters", out.framing.leftEndXMeters);
    out.framing.rightEndXMeters =
        framing.value("rightEndXMeters", out.framing.rightEndXMeters);
    out.framing.groundHeightMeters =
        framing.value("groundHeightMeters", out.framing.groundHeightMeters);
    out.framing.barrageSpawnTopYMeters = framing.value(
        "barrageSpawnTopYMeters", out.framing.barrageSpawnTopYMeters);
    out.framing.pivotZMeters =
        framing.value("pivotZMeters", out.framing.pivotZMeters);
  }
}

void ApplyEnemyPresentationFromJson(const nlohmann::json &enemyJson,
                                    EnemyParameters &out) {
  if (!enemyJson.is_object()) {
    return;
  }

  if (enemyJson.contains("panic") && enemyJson.at("panic").is_object()) {
    const auto &panic = enemyJson.at("panic");
    out.panic.lingerAfterAimSeconds = panic.value(
        "lingerAfterAimSeconds", out.panic.lingerAfterAimSeconds);
    out.panic.shakeAmplitudeHorizontalMeters = panic.value(
        "shakeAmplitudeHorizontalMeters",
        out.panic.shakeAmplitudeHorizontalMeters);
    out.panic.shakeAmplitudeVerticalMeters = panic.value(
        "shakeAmplitudeVerticalMeters", out.panic.shakeAmplitudeVerticalMeters);
    out.panic.wobbleSpeed = panic.value("wobbleSpeed", out.panic.wobbleSpeed);
    out.panic.sweatEmitIntervalSeconds = panic.value(
        "sweatEmitIntervalSeconds", out.panic.sweatEmitIntervalSeconds);
  }

  if (enemyJson.contains("periodicCharge") &&
      enemyJson.at("periodicCharge").is_object()) {
    const auto &charge = enemyJson.at("periodicCharge");
    out.periodicCharge.shakeDurationSeconds = charge.value(
        "shakeDurationSeconds", out.periodicCharge.shakeDurationSeconds);
    out.periodicCharge.chargeDurationSeconds = charge.value(
        "chargeDurationSeconds", out.periodicCharge.chargeDurationSeconds);
    out.periodicCharge.shakeAmplitudeMeters = charge.value(
        "shakeAmplitudeMeters", out.periodicCharge.shakeAmplitudeMeters);
    out.periodicCharge.shakeAngularSpeed = charge.value(
        "shakeAngularSpeed", out.periodicCharge.shakeAngularSpeed);
  }

  if (enemyJson.contains("chainExplosion") &&
      enemyJson.at("chainExplosion").is_object()) {
    const auto &chain = enemyJson.at("chainExplosion");
    out.chainExplosion.shrinkDurationSeconds = chain.value(
        "shrinkDurationSeconds", out.chainExplosion.shrinkDurationSeconds);
  }

  if (enemyJson.contains("variants") && enemyJson.at("variants").is_array()) {
    out.variants.clear();
    for (const auto &v : enemyJson.at("variants")) {
      if (!v.is_object()) {
        continue;
      }
      EnemyVariantParameters variant{};
      variant.name = v.value("name", variant.name);
      variant.moveSpeedMultiplier =
          std::max(0.0f, v.value("moveSpeedMultiplier", 1.0f));
      variant.chargeIntervalSeconds = v.value("chargeIntervalSeconds", 0.0f);
      variant.chargeEnabled = v.value("chargeEnabled", true);
      variant.weaveAmplitudeMeters = v.value("weaveAmplitudeMeters", 0.0f);
      variant.weaveHz = v.value("weaveHz", 0.0f);
      if (v.contains("color") && v.at("color").is_array() &&
          v.at("color").size() >= 3) {
        variant.colorR = v.at("color").at(0).get<float>();
        variant.colorG = v.at("color").at(1).get<float>();
        variant.colorB = v.at("color").at(2).get<float>();
      }
      out.variants.push_back(variant);
    }
  }
}

void ApplyBossFromJson(const nlohmann::json &root, BossParameters &out) {
  if (!root.contains("boss") || !root.at("boss").is_object()) {
    return;
  }
  const auto &b = root.at("boss");
  out.maxHealth = std::max(1, b.value("maxHealth", out.maxHealth));
  out.bodyRadiusMeters =
      std::max(0.1f, b.value("bodyRadiusMeters", out.bodyRadiusMeters));
  out.moveSpeedMetersPerSecond =
      b.value("moveSpeedMetersPerSecond", out.moveSpeedMetersPerSecond);
  out.skillIntervalSeconds =
      b.value("skillIntervalSeconds", out.skillIntervalSeconds);
  out.appearSeconds = std::max(0.1f, b.value("appearSeconds", out.appearSeconds));
  out.deathSeconds = std::max(0.1f, b.value("deathSeconds", out.deathSeconds));
  out.bulletDamage = b.value("bulletDamage", out.bulletDamage);
  out.explosionDamage = b.value("explosionDamage", out.explosionDamage);
  out.stunSeconds = b.value("stunSeconds", out.stunSeconds);
  out.stunDamageMultiplier =
      b.value("stunDamageMultiplier", out.stunDamageMultiplier);
  out.enrageHealthRatio = b.value("enrageHealthRatio", out.enrageHealthRatio);
  out.enrageIntervalMultiplier =
      b.value("enrageIntervalMultiplier", out.enrageIntervalMultiplier);
  out.maxMinions = b.value("maxMinions", out.maxMinions);
  if (b.contains("color") && b.at("color").is_array() &&
      b.at("color").size() >= 3) {
    out.colorR = b.at("color").at(0).get<float>();
    out.colorG = b.at("color").at(1).get<float>();
    out.colorB = b.at("color").at(2).get<float>();
  }
  if (b.contains("charge") && b.at("charge").is_object()) {
    const auto &c = b.at("charge");
    out.chargeTelegraphSeconds =
        c.value("telegraphSeconds", out.chargeTelegraphSeconds);
    out.chargeDurationSeconds =
        c.value("durationSeconds", out.chargeDurationSeconds);
    out.chargeSpeedMetersPerSecond =
        c.value("speedMetersPerSecond", out.chargeSpeedMetersPerSecond);
  }
  if (b.contains("summon") && b.at("summon").is_object()) {
    const auto &c = b.at("summon");
    out.summonTelegraphSeconds =
        c.value("telegraphSeconds", out.summonTelegraphSeconds);
    out.summonCount = c.value("count", out.summonCount);
    out.summonRingRadiusMeters =
        c.value("ringRadiusMeters", out.summonRingRadiusMeters);
  }
  if (b.contains("nova") && b.at("nova").is_object()) {
    const auto &c = b.at("nova");
    out.novaTelegraphSeconds =
        c.value("telegraphSeconds", out.novaTelegraphSeconds);
    out.novaRadiusMeters = c.value("radiusMeters", out.novaRadiusMeters);
    out.novaKnockbackSpeedMetersPerSecond = c.value(
        "knockbackSpeedMetersPerSecond", out.novaKnockbackSpeedMetersPerSecond);
  }
}

void ApplyStageFromJson(const nlohmann::json &root, StageParameters &out) {
  if (!root.contains("stage") || !root.at("stage").is_object()) {
    return;
  }
  const auto &s = root.at("stage");
  if (s.contains("waveEnemyCounts") && s.at("waveEnemyCounts").is_array()) {
    out.waveEnemyCounts.clear();
    for (const auto &n : s.at("waveEnemyCounts")) {
      if (n.is_number_integer()) {
        out.waveEnemyCounts.push_back(std::max(0, n.get<int>()));
      }
    }
  }
  out.waveStartDelaySeconds =
      s.value("waveStartDelaySeconds", out.waveStartDelaySeconds);
  out.bossAppearDelaySeconds =
      s.value("bossAppearDelaySeconds", out.bossAppearDelaySeconds);
  out.waveSpawnRadiusMeters =
      s.value("waveSpawnRadiusMeters", out.waveSpawnRadiusMeters);
}

bool TryParseEnemy(const nlohmann::json &json, EnemyParameters &out) {
  if (!json.is_object()) {
    return false;
  }
  if (!json.contains("hitRadiusMeters") ||
      !json.contains("moveSpeedMetersPerSecond") ||
      !json.contains("chargeIntervalSeconds") ||
      !json.contains("knockbackSpeedMetersPerSecond") ||
      !json.contains("explosion")) {
    return false;
  }

  const float hitRadius = json.at("hitRadiusMeters").get<float>();
  if (hitRadius <= 0.0f) {
    return false;
  }
  out.hitRadiusMeters = hitRadius;

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

  ApplyEnemyPresentationFromJson(json, out);
  out.modelName = json.value("modelName", std::string{});

  return true;
}

} // namespace

ParametersRuntime &ParametersRuntime::Instance() {
  static ParametersRuntime instance;
  return instance;
}

bool ParametersRuntime::TryLoadFromJsonFile(const std::string &path) {
  loaded_ = false;
  arena_ = ArenaParameters{};
  player_ = PlayerParameters{};
  enemy_ = EnemyParameters{};
  camera_ = CombatCameraParameters{};
  boss_ = BossParameters{};
  stage_ = StageParameters{};

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
  ApplyArenaFromJson(json, arena_);
  ApplyCombatCameraFromJson(json, camera_);
  ApplyBossFromJson(json, boss_);
  ApplyStageFromJson(json, stage_);
  loaded_ = true;
  return true;
}

void ParametersRuntime::Unload() {
  loaded_ = false;
  arena_ = ArenaParameters{};
  player_ = PlayerParameters{};
  enemy_ = EnemyParameters{};
  camera_ = CombatCameraParameters{};
  boss_ = BossParameters{};
  stage_ = StageParameters{};
}

} // namespace Atrum::AtomF