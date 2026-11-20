#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum {

/// 通常は Stop。フェード遷移中は CommandChangeScene へ引き継ぎ、暗転中の瞬断を避ける
void ReleaseSceneGameBgmOnExitScene(Audio::Handle &sceneBgmHandle);

} // namespace Atrum