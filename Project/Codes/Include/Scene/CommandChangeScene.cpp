#include "CommandChangeScene.h"
#include "CommandScene.h"

/// <summary>
/// 指令類 > シーン遷移指令 > コンストラクタ
/// </summary>
CommandChangeScene::CommandChangeScene() {

	commandScene_ = CommandScene::GetInstance();
	nextScene_ = nullptr;
	changeSceneEffect_.release();
	isExecute_ = true;

}