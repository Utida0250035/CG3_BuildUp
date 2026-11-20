#include "Scene/SceneResultBgmMaintain.h"

#include "Scene/CommandChangeScene.h"
#include "System/PlaySe.h"

namespace Atrum {

namespace {

constexpr std::initializer_list<const char *> kResultBgmCandidates = {
    "./Assets/Audios/bgmResult.mp3", "./Resources/bgmResult.mp3"};

} // namespace

void MaintainSceneResultBgmIfNeeded(Audio::Handle &sceneBgmHandle) {
  AssignPendingFadeSyncedSceneBgm(sceneBgmHandle);
  SyncSceneBgmForChangeSceneFade(sceneBgmHandle);

  if (TryResumeSceneBgmAfterChangeSceneFade(sceneBgmHandle)) {
    return;
  }

  if (sceneBgmHandle.isSuccess && IsSceneBgmAudible(sceneBgmHandle)) {
    if (!CommandChangeScene::Instance()->GetIsExecute()) {
      float pitchRatio = 1.0f;
      if (GetSeFrequencyRatio(sceneBgmHandle, pitchRatio) &&
          pitchRatio < 0.99f) {
        SetBgmLoopPitch(sceneBgmHandle, 1.0f);
      }
      float volume = 0.0f;
      if (GetSeVolume(sceneBgmHandle, volume) && volume < 0.99f) {
        SetBgmLoopVolume(sceneBgmHandle, 1.0f);
      }
    }
    return;
  }

  if (CommandChangeScene::Instance()->GetIsExecute()) {
    return;
  }

  sceneBgmHandle = PlayBgmLoop(kResultBgmCandidates);
  if (sceneBgmHandle.isSuccess) {
    SetBgmLoopPitch(sceneBgmHandle, 1.0f);
    SetBgmLoopVolume(sceneBgmHandle, 1.0f);
  }
}

} // namespace Atrum