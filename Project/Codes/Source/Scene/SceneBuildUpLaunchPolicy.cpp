#include "Scene/SceneBuildUpLaunchPolicy.h"

#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneBuildUp.h"

#include <memory>

namespace Atrum::SceneBuildUpLaunchPolicy {

void QueueFadeToBuildUp(const std::string &sceneJsonPath, Scene *returnScene) {
  Scene *const nextScene =
      SceneBuildUpRegistry::Instance().Launch(sceneJsonPath, returnScene);
  if (nextScene == nullptr) {
    return;
  }
  CommandChangeScene::Instance()->Set(
      nextScene, std::make_unique<ChangeSceneFade>().release());
}

} // namespace Atrum::SceneBuildUpLaunchPolicy