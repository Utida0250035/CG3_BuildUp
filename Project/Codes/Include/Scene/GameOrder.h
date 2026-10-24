#pragma once
#include "Engine/AtrumEngine.h"
#include "Input/AbstractInput.h"
#include "Scene/CommandScene.h"
#include "Audio/Audio.h"
#include "Scene/CommandChangeScene.h"
#include "System/DeltaTime.h"
#include <memory>
#include <chrono>

namespace Atrum {

/// <summary>
/// ゲーム統制
/// </summary>
	class GameOrder final {

	private:

		FrameDeltaTime* frameDeltaTime_ = nullptr;

		Audio::Manager* audio_ = nullptr;

		/// <summary>
		/// シーン指令へのアクセス
		/// </summary>
		CommandScene* commandScene_;

		/// <summary>
		/// シーン遷移指令へのアクセス
		/// </summary>
		CommandChangeScene* commandChangeScene_;

		/// <summary>
		/// コンストラクタ
		/// </summary>
		GameOrder();

	public:

		/// <summary>
		/// 実行部分
		/// </summary>
		void Run() {

			// デルタタイムは AtrumEngine::Process() で計算済み
#ifdef USE_IMGUI
			Engine::Instance()->ImGuiNewFrame();
#endif
			AbstractInput::Instance()->Update();

			Scene* sceneNow = commandScene_->GetSceneNow();
			Atrum::Camera* camera = sceneNow->GetCamera();
			if (sceneNow->IsStageViewCameraLocked()) {
				// 戦闘シーンでは DebugCamera の毎フレーム操作や Draw 内での再配置を行わない
				camera->UpdateMatrix();
			} else {
				camera->Update();
			}

			if (!commandChangeScene_->GetIsExecute()) {
				// シーン遷移の処理中でなければ

				// 現在シーンの更新処理
				sceneNow->Update();

			}

			// シーン遷移の更新処理
			commandChangeScene_->Update();

			Engine::Instance()->SetCamera(sceneNow->GetCamera());

			// 現在シーンの描画処理（PreDraw / PostDraw を含む）
			sceneNow->Draw();

		}

		/// <summary>
		/// デストラクタ 空
		/// </summary>
		~GameOrder() = default;

		/// <summary>
		/// コピーコンストラクタの削除
		/// </summary>
		/// <param name="source"></param>
		GameOrder(const GameOrder& source) = delete;

		/// <summary>
		/// 代入演算子のオーバーロードの削除
		/// </summary>
		/// <param name="source"></param>
		/// <returns></returns>
		GameOrder& operator=(const GameOrder& source) = delete;

		/// <summary>
		///
		/// </summary>
		/// <returns> ゲーム統制の静的インスタンス </returns>
		static GameOrder* Instance() {

			static GameOrder instance;

			return &instance;

		}

	};

}