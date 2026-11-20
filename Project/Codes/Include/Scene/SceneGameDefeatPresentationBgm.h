#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum {

/// 撃破／撃墜演出中: フェード同期・再開のみ（新規 PlayBgmLoop は行わない）
void SyncSceneBgmDuringDefeatPresentation(Audio::Handle &sceneBgmHandle);

} // namespace Atrum