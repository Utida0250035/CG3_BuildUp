#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum {

/// リザルト離脱時: フェード中は BGM を CommandChangeScene へ引き継ぎ、即 Stop しない
void ReleaseSceneResultBgmOnExitDuringFade(Audio::Handle &sceneBgmHandle);

} // namespace Atrum