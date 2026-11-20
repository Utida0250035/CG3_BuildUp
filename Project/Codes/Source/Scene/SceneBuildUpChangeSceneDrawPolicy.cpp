#include "Scene/SceneBuildUpChangeSceneDrawPolicy.h"

#include "Scene/CommandChangeScene.h"

namespace Atrum::SceneBuildUpChangeSceneDrawPolicy {

void DrawTransitionWhenActive() {
  CommandChangeScene *const changeScene = CommandChangeScene::Instance();
  if (changeScene == nullptr || !changeScene->GetIsExecute()) {
    return;
  }
  changeScene->Draw();
}

} // namespace Atrum::SceneBuildUpChangeSceneDrawPolicy