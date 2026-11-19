#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum {

/// ボス撃破演出中: フェード同期・再開のみ（新規 PlayBgmLoop は行わない）
void SyncSceneBgmDuringBossDeathPresentation(Audio::Handle &sceneBgmHandle);

} // namespace Atrum