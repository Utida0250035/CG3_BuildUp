#include "Scene/SceneResultSubmitTransition.h"

#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneResult.h"
#include "Scene/SceneTitle.h"
#include "System/PlaySe.h"

namespace Atrum {

bool SceneResultSubmitTransition::TryBeginOnSubmit(SceneResult &scene) {
  if (scene.ResultSubmitTransitionConsumed()) {
    return false;
  }

  CommandChangeScene *const changeScene = CommandChangeScene::Instance();
  if (changeScene->GetIsExecute()) {
    return false;
  }

  if (!AbstractInput::Instance()->GetData().isSubmitTrigger) {
    return false;
  }

  scene.SetResultSubmitTransitionConsumed(true);
  PlaySubmitSe();

  changeScene->RegisterDeferredEnterSceneBgm(
      {"./Assets/Audios/bgmTitle.mp3", "./Resources/bgmTitle.mp3"});
  changeScene->Set(SceneTitle::Instance(),
                   std::make_unique<ChangeSceneFade>().release());
  return true;
}

} // namespace Atrum