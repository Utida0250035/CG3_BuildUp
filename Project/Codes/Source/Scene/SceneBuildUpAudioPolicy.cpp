#include "Scene/SceneBuildUpAudioPolicy.h"

#include "Scene/CommandChangeScene.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneGameExitBgmPolicy.h"
#include "System/PlaySe.h"

#include <cmath>

namespace Atrum {

namespace {

constexpr std::initializer_list<const char *> kBuildUpBgmCandidates = {
    "./Assets/Audios/bgmGame.mp3", "./Resources/bgmGame.mp3"};

constexpr std::initializer_list<const char *> kBuildUpAmbientLoopCandidates = {
    "./Assets/Audios/seTitleAmbientLoop.mp3",
    "./Resources/seTitleAmbientLoop.mp3"};

void MaintainBuildUpAmbientLoop(const Audio::Handle &ambientHandle) {
  if (!ambientHandle.isSuccess) {
    return;
  }
  const bool fading = CommandChangeScene::Instance()->GetIsExecute();
  const float targetVolume = fading ? 0.08f : 0.28f;
  const float targetPitch = fading ? 0.9f : 0.98f;

  float currentVolume = targetVolume;
  if (!GetSeVolume(ambientHandle, currentVolume) ||
      std::abs(currentVolume - targetVolume) > 0.02f) {
    SetSeVolume(ambientHandle, targetVolume);
  }
  float currentPitch = targetPitch;
  if (!GetSeFrequencyRatio(ambientHandle, currentPitch) ||
      std::abs(currentPitch - targetPitch) > 0.02f) {
    SetSeFrequencyRatio(ambientHandle, targetPitch);
  }
}

} // namespace

void SceneBuildUpAudioPolicy::OnEnterScene(SceneBuildUp &scene) {
  if (scene.SceneDocumentLoadFailed()) {
    return;
  }

  scene.SceneBgmHandle() =
      PlayBgmLoop(kBuildUpBgmCandidates);
  if (scene.SceneBgmHandle().isSuccess) {
    SetBgmLoopPitch(scene.SceneBgmHandle(), 1.0f);
  }

  scene.SceneAmbientLoopHandle() =
      PlaySeLoopCandidates(kBuildUpAmbientLoopCandidates);
  if (scene.SceneAmbientLoopHandle().isSuccess) {
    SetSeVolume(scene.SceneAmbientLoopHandle(), 0.28f);
    SetSeFrequencyRatio(scene.SceneAmbientLoopHandle(), 0.98f);
  }

  if (!CommandChangeScene::Instance()->GetIsExecute()) {
    PlaySceneTransitionSe();
  }
}

void SceneBuildUpAudioPolicy::Tick(SceneBuildUp &scene) {
  if (scene.SceneDocumentLoadFailed()) {
    return;
  }

  Audio::Handle &bgm = scene.SceneBgmHandle();
  AssignPendingFadeSyncedSceneBgm(bgm);
  SyncSceneBgmForChangeSceneFade(bgm);
  TryResumeSceneBgmAfterChangeSceneFade(bgm);

  if (bgm.isSuccess && !CommandChangeScene::Instance()->GetIsExecute()) {
    float pitchRatio = 1.0f;
    if (GetSeFrequencyRatio(bgm, pitchRatio) && pitchRatio < 0.99f) {
      SetBgmLoopPitch(bgm, 1.0f);
    }
  }

  MaintainBuildUpAmbientLoop(scene.SceneAmbientLoopHandle());
}

void SceneBuildUpAudioPolicy::OnExitScene(SceneBuildUp &scene) {
  ReleaseSceneBgmOnExitDuringFade(scene.SceneBgmHandle());
  StopSeLoop(scene.SceneAmbientLoopHandle());
  scene.SceneAmbientLoopHandle() = {};
}

} // namespace Atrum