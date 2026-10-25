#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"

namespace Atrum {

/// <summary>
/// 指令類 > シーン遷移指令 > コンストラクタ
/// </summary>
	CommandChangeScene::CommandChangeScene() {

		commandScene_ = CommandScene::Instance();
		nextScene_ = nullptr;
		changeSceneEffect_.reset();
		isExecute_ = false;

	}

}