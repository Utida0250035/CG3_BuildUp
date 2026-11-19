#include "Scene/SceneGameDefeatPresentationBgm.h"

#include "System/PlaySe.h"

namespace Atrum {

void SyncSceneBgmDuringBossDeathPresentation(Audio::Handle &sceneBgmHandle) {
  AssignPendingFadeSyncedSceneBgm(sceneBgmHandle);
  (void)TryResumeSceneBgmAfterChangeSceneFade(sceneBgmHandle);
}

} // namespace Atrum