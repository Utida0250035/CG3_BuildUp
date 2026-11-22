#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Debug {

void SetFrameDeltaTimeOverlayVisible(const bool visible);
bool IsFrameDeltaTimeOverlayVisible();

void SetFrameDeltaTimeHistogramVisible(const bool visible);
bool IsFrameDeltaTimeHistogramVisible();

/// オーバーレイ（既定 ON）とヒストグラム窓を描画
void DrawFrameDeltaTimeDebug();

} // namespace Atrum::Debug

#endif