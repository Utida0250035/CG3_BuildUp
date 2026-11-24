#include "Scene/CommandScene.h"

#if defined(DEVELOPMENT)
#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/CommandSceneDevelopmentBootApply.h"
#else
#include "Scene/CommandSceneReleaseBootApply.h"
#endif
#include "Scene/CommandSceneBootEnterApply.h"

namespace Atrum {

/// <summary>
/// 指令類 > シーン指令 > コンストラクタ
/// </summary>
CommandScene::CommandScene() {

}

bool CommandScene::Initialize() {

#if defined(DEVELOPMENT)
	const DevelopmentBootTarget bootTarget = ResolveDevelopmentBootTarget();
	sceneNow_ = CreateInitialSceneForDevelopmentBoot(bootTarget);
#else
	sceneNow_ = CreateInitialSceneForReleaseBoot();
#endif

	sceneNow_ = PrepareInitialSceneBeforeEnter(sceneNow_);
	EnterInitialSceneNow(sceneNow_);

	return true;

}

} // namespace Atrum