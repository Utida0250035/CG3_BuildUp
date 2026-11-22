#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum {

/// リザルト BGM のフェード同期・途切れ検知・再開
void MaintainSceneResultBgmIfNeeded(Audio::Handle &sceneBgmHandle);

} // namespace Atrum