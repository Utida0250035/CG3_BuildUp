#include "Scene/CommandSceneReleaseBootApply.h"

#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#include "Scene/SceneTitle.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum {

Scene *CreateInitialSceneForReleaseBoot() {
  const std::string startupScene = Editor::Scene::ResolveStartupScenePath();
  if (!startupScene.empty() && IsValidBootSceneJsonPath(startupScene)) {
    return SceneBuildUpLaunchPolicy::CreateOwnedBuildUp(startupScene,
                                                        SceneTitle::Instance());
  }
  return SceneTitle::Instance();
}

} // namespace Atrum