#pragma once
#include "Camera/ActiveCameraSystem.h"
#include "Engine/AtrumEngine.h"
#include "Input/AbstractInput.h"
#include "Scene/CommandScene.h"
#include "Audio/Audio.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/GameOrderAudioPolicy.h"
#include "System/DeltaTime.h"
#if defined(DEVELOPMENT)
#include "ForDebug/SubsystemFrameProfiler.h"
#endif
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/AudioStreamingDebugImGui.h"
#include "ForDebug/LightingEffectsDebugImGui.h"
#include "ForDebug/FrameDeltaTimeDebugImGui.h"
#endif
#if !defined(DEVELOPMENT) && !defined(USE_IMGUI)
#include "Scene/SceneReleaseCarousel.h"
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

		bool Initialize();

		/// <summary>
		/// 実行部分
		/// </summary>
		void Run() {

			// デルタタイムは AtrumEngine::Process() で計算済み
#if defined(DEVELOPMENT)
			Atrum::Debug::SubsystemFrameProfilerBeginFrame();
#endif
			{
#if defined(DEVELOPMENT)
				Atrum::Debug::SubsystemFrameScope imguiScope(
				    Atrum::Debug::SubsystemFrameId::ImGuiPrep);
#endif
#ifdef USE_IMGUI
				Engine::Instance()->ImGuiNewFrame();
#if defined(DEVELOPMENT)
				Atrum::Debug::DrawAudioStreamingMemoryDebugWindow();
				Atrum::Debug::DrawLightingEffectsDebugWindow();
				Atrum::Debug::DrawFrameDeltaTimeDebug();
				Atrum::Debug::DrawSubsystemFrameProfilerDebug();
#endif
#endif
			}

			{
#if defined(DEVELOPMENT)
				Atrum::Debug::SubsystemFrameScope inputScope(
				    Atrum::Debug::SubsystemFrameId::Input);
#endif
				AbstractInput::Instance()->Update();
			}

			Scene* sceneNow = commandScene_->GetSceneNow();

			if (commandChangeScene_->GetIsExecute() &&
			    !commandChangeScene_->AllowsCurrentSceneUpdate()) {
				sceneNow->TryHandleInputWhileSceneTransitionBlocked();
			}

			{
#if defined(DEVELOPMENT)
				Atrum::Debug::SubsystemFrameScope updateScope(
				    Atrum::Debug::SubsystemFrameId::SceneUpdate);
#endif
				if (commandChangeScene_->AllowsCurrentSceneUpdate()) {
					sceneNow->Update();
				}
			}

			// シーン遷移の更新処理（遷移先が非シングルトンの場合、直前の scene は破棄される）
			{
#if defined(DEVELOPMENT)
				Atrum::Debug::SubsystemFrameScope transitionScope(
				    Atrum::Debug::SubsystemFrameId::SceneTransition);
#endif
				commandChangeScene_->Update();
			}

			sceneNow = commandScene_->GetSceneNow();

			{
#if defined(DEVELOPMENT)
				Atrum::Debug::SubsystemFrameScope audioScope(
				    Atrum::Debug::SubsystemFrameId::Audio);
#endif
				TickGameOrderFrameAudio(audio_, commandChangeScene_);
			}

			{
#if defined(DEVELOPMENT)
				Atrum::Debug::SubsystemFrameScope cameraScope(
				    Atrum::Debug::SubsystemFrameId::CameraPrepare);
#endif
				ActiveCameraSystem::Instance().PrepareSceneAndBindEngine(
				    sceneNow, Engine::Instance());
			}

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