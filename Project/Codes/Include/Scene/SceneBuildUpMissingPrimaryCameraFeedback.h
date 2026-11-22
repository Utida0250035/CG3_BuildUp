#pragma once

#include <string>

namespace Atrum {

class AtrumEngine;

std::string BuildSceneBuildUpMissingPrimaryCameraMessage(
    const std::string &sceneJsonPath);

/// プライマリカメラ欠落時の暗転オーバーレイ（空画面に見えないようにする）
void DrawSceneBuildUpMissingPrimaryCameraDimOverlay(AtrumEngine *engine);

/// true を返したら呼び手は通常描画を打ち切る
bool HandleSceneBuildUpDrawWithoutPrimaryCamera(AtrumEngine *engine,
                                                const std::string &sceneJsonPath);

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
void DrawSceneBuildUpMissingPrimaryCameraNoticeIfPending();
#endif

} // namespace Atrum