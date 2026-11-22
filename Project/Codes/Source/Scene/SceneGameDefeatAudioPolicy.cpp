#include "Scene/SceneGameDefeatAudioPolicy.h"

#include "System/PlaySe.h"

namespace Atrum {

void BeginBossDefeatSceneBgmPolicy(Audio::Handle &sceneBgmHandle) {
  (void)sceneBgmHandle;
}

void BeginPlayerDefeatSceneBgmPolicy(Audio::Handle &sceneBgmHandle) {
  if (!sceneBgmHandle.isSuccess) {
    return;
  }
  constexpr float kDefeatSlowPitch = 0.82f;
  float pitchRatio = 1.0f;
  if (!GetSeFrequencyRatio(sceneBgmHandle, pitchRatio) ||
      pitchRatio > kDefeatSlowPitch + 0.02f) {
    SetSeFrequencyRatio(sceneBgmHandle, kDefeatSlowPitch);
  }
}

void StopSceneBgmAfterBossVictoryPresentation(Audio::Handle &sceneBgmHandle) {
  StopBgmLoop(sceneBgmHandle);
  sceneBgmHandle = {};
}

} // namespace Atrum