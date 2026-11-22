#pragma once

#include <cstdint>

#include "Engine/Renderer/Lighting/DirectionalShadow.h"

namespace Atrum::Visual::DirectionalShadowApplyDiagnostics {

enum class PlanarDrawSkipReason : uint8_t {
  None = 0,
  NullEngine,
  PlanarDisabled,
  NonPositiveRadius,
};

struct ApplySnapshot {
  uint32_t shadeEnabled = 0;
  uint32_t planarEnabled = 0;
  uint32_t castShadowFlag = 0;
  uint64_t planarDrawCount = 0;
  uint64_t planarSkipCount = 0;
  PlanarDrawSkipReason lastPlanarSkip = PlanarDrawSkipReason::None;
  uint64_t lightDataMergeCount = 0;
};

void ResetFrameCounters();

void RecordPlanarGroundShadowDraw();
void RecordPlanarGroundShadowSkip(const PlanarDrawSkipReason reason);

void RecordDirectionalLightDataMerge(const DirectionalShadowSettings &settings,
                                     uint32_t directionalLightFlags);

ApplySnapshot GetSnapshot();

const char *PlanarDrawSkipReasonLabel(const PlanarDrawSkipReason reason);

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

/// ライティング効果パネル内の適用状態・描画統計オーバーレイ。
void DrawApplyStateOverlay(const DirectionalShadowSettings &settings);

#endif

} // namespace Atrum::Visual::DirectionalShadowApplyDiagnostics