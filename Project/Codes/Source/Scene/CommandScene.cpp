#include "Scene/CommandScene.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/CommandSceneDevelopmentBootApply.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#else
#include "Scene/CommandSceneReleaseBootApply.h"
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
  sceneNow_ = CreateInitialSceneForReleaseBoot();
#endif

  sceneNow_->EnterScene();
}

} // namespace Atrum