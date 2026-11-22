#include "Camera/StageViewFramingBounds.h"

#include "System/StageBounds.h"

#include <nlohmann/json.hpp>

namespace Atrum {

namespace {

StageViewFramingBounds g_activeBounds = StageViewFramingBounds::DefaultFromStageConstants();
bool g_hasPublishedBounds = false;

} // namespace

StageViewFramingBounds StageViewFramingBounds::DefaultFromStageConstants() {
  StageViewFramingBounds bounds{};
  bounds.leftEndX = kLeftEndX;
  bounds.rightEndX = kRightEndX;
  bounds.groundHeight = kGroundHeight;
  bounds.barrageSpawnTopY = kBarrageSpawnTopY;
  bounds.pivotZ = 0.0f;
  return bounds;
}

bool StageViewFramingBounds::TryParse(const nlohmann::json &j,
                                     StageViewFramingBounds &out) {
  if (!j.is_object()) {
    return false;
  }

  StageViewFramingBounds bounds = DefaultFromStageConstants();
  if (j.contains("leftEndX")) {
    bounds.leftEndX = j.at("leftEndX").get<float>();
  }
  if (j.contains("rightEndX")) {
    bounds.rightEndX = j.at("rightEndX").get<float>();
  }
  if (j.contains("groundHeight")) {
    bounds.groundHeight = j.at("groundHeight").get<float>();
  }
  if (j.contains("barrageSpawnTopY")) {
    bounds.barrageSpawnTopY = j.at("barrageSpawnTopY").get<float>();
  }
  if (j.contains("pivotZ")) {
    bounds.pivotZ = j.at("pivotZ").get<float>();
  }

  if (bounds.rightEndX <= bounds.leftEndX) {
    return false;
  }
  if (bounds.barrageSpawnTopY <= bounds.groundHeight) {
    return false;
  }

  out = bounds;
  return true;
}

const StageViewFramingBounds &ResolveStageViewFramingBounds() {
  if (g_hasPublishedBounds) {
    return g_activeBounds;
  }
  // 未公開時も参照の寿命を呼び出し側に残す。
  static const StageViewFramingBounds defaultBounds =
      StageViewFramingBounds::DefaultFromStageConstants();
  return defaultBounds;
}

void PublishStageViewFramingBounds(const StageViewFramingBounds &bounds) {
  g_activeBounds = bounds;
  g_hasPublishedBounds = true;
}

} // namespace Atrum