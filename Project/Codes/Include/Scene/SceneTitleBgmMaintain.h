#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum {

/// タイトル BGM のフェード同期・途切れ検知・再開
void MaintainSceneTitleBgmIfNeeded(Audio::Handle &sceneBgmHandle);

/// タイトル環境ループ SE の途切れ検知・再開（フェード中は Update 側のダックを維持）
void MaintainSceneTitleAmbientLoopIfNeeded(Audio::Handle &ambientLoopHandle);

} // namespace Atrum