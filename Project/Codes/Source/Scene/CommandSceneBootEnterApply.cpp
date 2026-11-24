#include "Scene/CommandSceneBootEnterApply.h"

#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneTitle.h"

namespace Atrum {

Scene *PrepareInitialSceneBeforeEnter(Scene *sceneNow) {
  if (sceneNow == nullptr) {
    return SceneTitle::Instance();
  }

#if !defined(DEVELOPMENT)
  SceneBuildUp *const buildUp = dynamic_cast<SceneBuildUp *>(sceneNow);
  if (buildUp != nullptr &&
      !IsValidBootSceneJsonPath(buildUp->SceneJsonPath())) {
    return SceneTitle::Instance();
  }
#endif

  return sceneNow;
}

void EnterInitialSceneNow(Scene *sceneNow) {
  if (sceneNow == nullptr) {
    return;
  }
  sceneNow->EnterScene();
}

} // namespace Atrum