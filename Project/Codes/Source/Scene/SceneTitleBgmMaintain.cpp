#include "Scene/SceneTitleBgmMaintain.h"

#include "Audio/Audio.h"
#include "Engine/EngineAccess.h"
#include "Scene/CommandChangeScene.h"
#include "System/PlaySe.h"

namespace Atrum {

namespace {

constexpr std::initializer_list<const char *> kTitleBgmCandidates = {
    "./Assets/Audios/bgmTitle.mp3", "./Resources/bgmTitle.mp3"};

constexpr std::initializer_list<const char *> kTitleAmbientLoopCandidates = {
    "./Assets/Audios/seTitleAmbientLoop.mp3",
    "./Resources/seTitleAmbientLoop.mp3"};

bool IsLoopSePlaying(const Audio::Handle &handle) {
  if (!handle.isSuccess) {
    return false;
  }
  return ::Atrum::EngineAudioManager()->IsPlaying(handle);
}

} // namespace

void MaintainSceneTitleBgmIfNeeded(Audio::Handle &sceneBgmHandle) {
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
    }
    return;
  }

  if (CommandChangeScene::Instance()->GetIsExecute()) {
    return;
  }

  sceneBgmHandle = PlayBgmLoop(kTitleBgmCandidates);
  if (sceneBgmHandle.isSuccess) {
    SetBgmLoopPitch(sceneBgmHandle, 1.0f);
    SetBgmLoopVolume(sceneBgmHandle, 1.0f);
  }
}

void MaintainSceneTitleAmbientLoopIfNeeded(Audio::Handle &ambientLoopHandle) {
  if (CommandChangeScene::Instance()->GetIsExecute()) {
    return;
  }

  if (ambientLoopHandle.isSuccess && IsLoopSePlaying(ambientLoopHandle)) {
    return;
  }

  ambientLoopHandle = PlaySeLoopCandidates(kTitleAmbientLoopCandidates);
  if (ambientLoopHandle.isSuccess) {
    SetSeVolume(ambientLoopHandle, 0.42f);
    SetSeFrequencyRatio(ambientLoopHandle, 0.98f);
  }
}

} // namespace Atrum