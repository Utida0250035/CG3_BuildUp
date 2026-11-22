#include "Scene/SceneTitleSubmitTransition.h"

#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneMovie.h"
#include "Scene/SceneTitle.h"
#include "System/GameAssetsPaths.h"
#include "System/PlaySe.h"

namespace Atrum {

bool SceneTitleSubmitTransition::TryBeginOnSubmit(SceneTitle &scene) {
  if (scene.TitleSubmitTransitionConsumed()) {
    return false;
  }

  CommandChangeScene *const changeScene = CommandChangeScene::Instance();
  if (changeScene->GetIsExecute()) {
    return false;
  }

  const InputData &input = AbstractInput::Instance()->GetData();
  if (!input.isSubmitTrigger) {
    return false;
  }

  scene.SetTitleSubmitTransitionConsumed(true);
  PlaySubmitSe();

  Scene *const nextAfterMovie = SceneGame::Instance();
  Scene *const introMovie = SceneMovieRegistry::Instance().Launch(
      GameAssets::kTitleIntroMovieTrack, nextAfterMovie);
  changeScene->Set(introMovie, std::make_unique<ChangeSceneFade>().release());
  return true;
}

} // namespace Atrum