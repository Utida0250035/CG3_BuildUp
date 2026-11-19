#pragma once

#include <nlohmann/json_fwd.hpp>

namespace Atrum {

/// StageView 戦闘カメラのフレーミング（XY プレイエリア上端まで）
struct StageViewFramingBounds {
  float leftEndX = -14.0f;
  float rightEndX = 14.0f;
  float groundHeight = -4.0f;
  float barrageSpawnTopY = 9.25f;
  float pivotZ = 0.0f;

  static StageViewFramingBounds DefaultFromStageConstants();

  static bool TryParse(const nlohmann::json &j, StageViewFramingBounds &out);
};

/// 現行シーンのフレーミング（戦闘カメラ計算の共有コンテキスト）
const StageViewFramingBounds &ResolveStageViewFramingBounds();
void PublishStageViewFramingBounds(const StageViewFramingBounds &bounds);

} // namespace Atrum