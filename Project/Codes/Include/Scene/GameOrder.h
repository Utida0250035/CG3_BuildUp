#pragma once
#include "Camera/ActiveCameraSystem.h"
#include "Engine/AtrumEngine.h"
#include "Input/AbstractInput.h"
#include "Scene/CommandScene.h"
#include "Audio/Audio.h"
#include "Scene/CommandChangeScene.h"
#include "System/DeltaTime.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/AudioStreamingDebugImGui.h"
#endif
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
#if defined(DEVELOPMENT)
			Atrum::Debug::DrawAudioStreamingMemoryDebugWindow();
#endif
#endif
			AbstractInput::Instance()->Update();

			Scene* sceneNow = commandScene_->GetSceneNow();

			if (!commandChangeScene_->GetIsExecute()) {
#if !defined(DEVELOPMENT) && !defined(USE_IMGUI)
				if (!Atrum::SceneFlow::SceneReleaseCarousel::Instance()
				        .TryAdvanceOnSubmit()) {
					sceneNow->Update();
				}
#else
				sceneNow->Update();
#endif
			}

			// シーン遷移の更新処理（遷移先が非シングルトンの場合、直前の scene は破棄される）
			commandChangeScene_->Update();

			sceneNow = commandScene_->GetSceneNow();

			ActiveCameraSystem::Instance().PrepareSceneAndBindEngine(
			    sceneNow, Engine::Instance());

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