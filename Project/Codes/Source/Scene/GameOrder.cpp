#include "Scene/GameOrder.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"
#include <chrono>

namespace Atrum {

	/// <summary>
	/// ゲーム統制 > コンストラクタ
	/// </summary>
	GameOrder::GameOrder() {

		frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();
		audio_ = ::Atrum::EngineAudioManager();

		// シーン指令のインスタンス取得
		commandScene_ = CommandScene::Instance();

		commandChangeScene_ = CommandChangeScene::Instance();

		AbstractInput::Instance()->Initialize();

	}

}