#include "Scene/CommandScene.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/CommandSceneDevelopmentBootApply.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#else
#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#include "Scene/SceneTitle.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#endif

namespace Atrum {

/// <summary>
/// 指令類 > シーン指令 > コンストラクタ
/// </summary>
CommandScene::CommandScene() {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  const DevelopmentBootTarget bootTarget = ResolveDevelopmentBootTarget();
  sceneNow_ = CreateInitialSceneForDevelopmentBoot(bootTarget);
#else
  const std::string startupScene = Editor::Scene::ResolveStartupScenePath();
  if (!startupScene.empty() && IsValidBootSceneJsonPath(startupScene)) {
    sceneNow_ = SceneBuildUpLaunchPolicy::CreateOwnedBuildUp(
        startupScene, SceneTitle::Instance());
  } else if (IsValidBootSceneJsonPath(EditorAssets::kDebugPlayAreaTemplateScene)) {
    sceneNow_ = SceneBuildUpLaunchPolicy::CreateOwnedBuildUp(
        EditorAssets::kDebugPlayAreaTemplateScene, SceneTitle::Instance());
  } else {
    sceneNow_ = SceneTitle::Instance();
  }
#endif

  sceneNow_->EnterScene();
}

} // namespace Atrum