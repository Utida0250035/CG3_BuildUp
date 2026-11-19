#include "Scene/SceneMovieAudioPolicy.h"

#include "Scene/CommandChangeScene.h"
#include "Scene/SceneEntityEditor.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneTitle.h"

namespace Atrum {

void SceneMovieAudioPolicy::OnEnterScene() {
  // 方針: シーン遷移 SE は ChangeSceneFade が担当。ムービー中はループ BGM 無し。
}

void SceneMovieAudioPolicy::TickDuringMovie(const bool hasPendingSceneChange) {
  if (hasPendingSceneChange) {
    return;
  }
  // 方針: トラック再生中も呼び出し元のフェード同期 BGM を奪わない（スロット未保持）。
}

void SceneMovieAudioPolicy::PrepareReturnSceneBgmBeforeFade(Scene *returnScene) {
  if (returnScene == nullptr) {
    return;
  }
  if (!kReturnSceneOwnsBgmRestart) {
    return;
  }

  CommandChangeScene *const changeScene = CommandChangeScene::Instance();
  if (returnScene == SceneGame::Instance()) {
    changeScene->RegisterDeferredEnterSceneBgm(
        {"./Assets/Audios/bgmGame.mp3", "./Resources/bgmGame.mp3"});
    return;
  }
  if (returnScene == SceneTitle::Instance()) {
    changeScene->RegisterDeferredEnterSceneBgm(
        {"./Assets/Audios/bgmTitle.mp3", "./Resources/bgmTitle.mp3"});
    return;
  }
  if (returnScene == SceneEntityEditor::Instance()) {
    // エディタ作業シーン: ループ BGM なし
    return;
  }
}

void SceneMovieAudioPolicy::OnExitScene() {
  // ムービーローカルの SE ハンドルが増えたらここで停止
}

} // namespace Atrum