#include "Scene/GameOrder.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"
#include <chrono>

namespace Atrum {

	/// <summary>
	/// ゲーム統制 > コンストラクタ
	/// </summary>
	GameOrder::GameOrder() {

		frameDeltaTime_ = FrameDeltaTime::Instance();

		frameDeltaTime_->Initialize();

		audio_ = Audio::Manager::Instance();

		// シーン指令のインスタンス取得
		commandScene_ = CommandScene::Instance();

		commandChangeScene_ = CommandChangeScene::Instance();

		AbstractInput::Instance()->Initialize();

	}

}