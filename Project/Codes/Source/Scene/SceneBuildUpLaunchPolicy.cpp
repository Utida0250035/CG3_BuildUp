#include "Scene/SceneBuildUpLaunchPolicy.h"

#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneBuildUp.h"

#include <memory>

namespace Atrum::SceneBuildUpLaunchPolicy {

Scene *CreateOwnedBuildUp(const std::string &sceneJsonPath, Scene *returnScene) {
  return SceneBuildUpRegistry::Instance().Launch(sceneJsonPath, returnScene);
}

void QueueFadeToBuildUp(const std::string &sceneJsonPath, Scene *returnScene) {
  Scene *const nextScene = CreateOwnedBuildUp(sceneJsonPath, returnScene);
  if (nextScene == nullptr) {
    return;
  }
  CommandChangeScene::Instance()->Set(
      nextScene, std::make_unique<ChangeSceneFade>().release());
}

} // namespace Atrum::SceneBuildUpLaunchPolicy