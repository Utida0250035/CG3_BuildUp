#pragma once

namespace Atrum {

class CameraGameIntroController;

/// イントロ演出中は EnterScene で開始した BGM を維持し、PlayBgmLoop 再発火を抑止する
bool ShouldDeferSceneBgmRestartDuringIntro(
    const CameraGameIntroController *introController);

} // namespace Atrum