#include "GameOrder.h"
#include "CommandChangeScene.h"
#include "CommandScene.h"
#include <chrono>

/// <summary>
/// ゲーム統制 > コンストラクタ
/// </summary>
GameOrder::GameOrder() {

	// プレイヤー入力のインスタンス取得
	playerInput_ = PlayerInput::GetInstance();

	// シーン指令のインスタンス取得
	commandScene_ = CommandScene::GetInstance();

	commandChangeScene_ = CommandChangeScene::GetInstance();

	// 現在時間を設定
	preTime_ = std::chrono::steady_clock::now();

	// 現在時間をコピー
	currentTime_ = preTime_;

	// デルタタイムに0マイクロ秒を設定
	deltaTime_ = std::chrono::duration<float>::zero();

}