#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum {

/// ボス撃破開始: BGM は止めず演出完了まで維持（即 Stop しない）
void BeginBossDefeatSceneBgmPolicy(Audio::Handle &sceneBgmHandle);

/// 自機撃墜開始: BGM を止めずテンポだけ落とす
void BeginPlayerDefeatSceneBgmPolicy(Audio::Handle &sceneBgmHandle);

/// ボス撃破演出後・リザルト遷移直前
void StopSceneBgmAfterBossVictoryPresentation(Audio::Handle &sceneBgmHandle);

} // namespace Atrum