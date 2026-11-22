#include "Scene/SceneGameExitBgmPolicy.h"

#include "Scene/CommandChangeScene.h"
#include "System/PlaySe.h"

namespace Atrum {

void ReleaseSceneBgmOnExitDuringFade(Audio::Handle &sceneBgmHandle) {
  if (!sceneBgmHandle.isSuccess) {
    return;
  }

  CommandChangeScene *const changeScene = CommandChangeScene::Instance();
  if (changeScene != nullptr && changeScene->GetIsExecute()) {
    changeScene->AdoptExitingSceneBgmPendingFadeStop(sceneBgmHandle);
    return;
  }

  StopBgmLoop(sceneBgmHandle);
  sceneBgmHandle = {};
}

} // namespace Atrum