#include "CommandScene.h"
#include "SceneTitle.h"

/// <summary>
/// 指令類 > シーン指令 > コンストラクタ
/// </summary>
CommandScene::CommandScene() {

	// 現在シーンにタイトルシーンを設定
	sceneNow_ = SceneTitle::GetInstance();

	// 現在シーンの進入処理
	sceneNow_->EnterScene();

}