#include "Scene/SceneTitle.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Scene/SceneTitleVisualApply.h"
#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneMovie.h"
#include "System/GameAssetsPaths.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "Scene/SceneGameExitBgmPolicy.h"
#include "Scene/SceneTitleBgmMaintain.h"
#include "Scene/SceneTitleSubmitTransition.h"
#include "Scene/SceneScreenUi.h"
#include "System/PlaySe.h"

#include <cmath>

namespace Atrum {

	void SceneTitle::EnterScene() {
		titleSubmitTransitionConsumed_ = false;
		EnsureCommonResources();
		SetupStageViewCamera();
		ApplySceneTitleVisualFromProfile();
		Atrum::SceneScreenUi::PreloadTitleUiTextures();
		SpawnBackgrounds();
		sceneBgmHandle_ = Atrum::PlayBgmLoop({ "./Assets/Audios/bgmTitle.mp3",
											  "./Resources/bgmTitle.mp3" });
		if (sceneBgmHandle_.isSuccess) {
			float pitchRatio = 1.0f;
			if (!Atrum::GetSeFrequencyRatio(sceneBgmHandle_, pitchRatio) ||
			    pitchRatio < 0.99f) {
				Atrum::SetSeFrequencyRatio(sceneBgmHandle_, 1.0f);
			}
		}
		sceneAmbientLoopSeHandle_ = Atrum::PlaySeLoopCandidates(
			{"./Assets/Audios/seTitleAmbientLoop.mp3",
			 "./Resources/seTitleAmbientLoop.mp3"});
		if (sceneAmbientLoopSeHandle_.isSuccess) {
			Atrum::SetSeVolume(sceneAmbientLoopSeHandle_, 0.42f);
			Atrum::SetSeFrequencyRatio(sceneAmbientLoopSeHandle_, 0.98f);
		}
	}

	void SceneTitle::Update() {
		Atrum::MaintainSceneTitleBgmIfNeeded(sceneBgmHandle_);
		Atrum::MaintainSceneTitleAmbientLoopIfNeeded(sceneAmbientLoopSeHandle_);
		if (sceneAmbientLoopSeHandle_.isSuccess) {
			const bool fading =
				Atrum::CommandChangeScene::Instance()->GetIsExecute();
			const float targetVolume = fading ? 0.1f : 0.42f;
			const float targetPitch = fading ? 0.9f : 0.98f;
			float currentVolume = targetVolume;
			if (!Atrum::GetSeVolume(sceneAmbientLoopSeHandle_, currentVolume) ||
				std::abs(currentVolume - targetVolume) > 0.02f) {
				Atrum::SetSeVolume(sceneAmbientLoopSeHandle_, targetVolume);
			}
			float currentPitch = targetPitch;
			if (!Atrum::GetSeFrequencyRatio(sceneAmbientLoopSeHandle_,
			                                currentPitch) ||
				std::abs(currentPitch - targetPitch) > 0.02f) {
				Atrum::SetSeFrequencyRatio(sceneAmbientLoopSeHandle_, targetPitch);
			}
		}
		UpdateBackgrounds();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
		// エディタビルドの起動シーンは SceneEntityEditor（タイトル経由の遷移は無効）
		return;
#else
		(void)SceneTitleSubmitTransition::TryBeginOnSubmit(*this);
#endif
	}

	void SceneTitle::DrawScene() {
		ApplySceneTitleVisualFromProfile();
		DrawBackgrounds();

		if (Atrum::AtrumEngine* engine = ::Atrum::EngineInstance()) {
			Atrum::SceneScreenUi::DrawTitleSceneUi(engine, GetCamera());
		}
	}

	void SceneTitle::ExitScene() {
		ReleaseCommonResources();
		Atrum::ReleaseSceneBgmOnExitDuringFade(sceneBgmHandle_);
		Atrum::StopSeLoop(sceneAmbientLoopSeHandle_);
		sceneAmbientLoopSeHandle_ = {};
		ClearBackgrounds();
	}

	void SceneTitle::SpawnBackgrounds() {
		ClearBackgrounds();
		skyDome_ = Atrum::EntityFactory::SkyDome(GetCamera());
		backgrounds_ = Atrum::EntityFactory::ScrollSpheres();
		foregroundGroundClouds_ = Atrum::EntityFactory::ForegroundGroundClouds();
	}

	void SceneTitle::ClearBackgrounds() {
		if (skyDome_) {
			skyDome_->Finalize();
			skyDome_.reset();
		}

		for (std::unique_ptr<Atrum::Entity>& background : backgrounds_) {
			if (background) {
				background->Finalize();
			}
		}
		backgrounds_.clear();

		for (std::unique_ptr<Atrum::Entity>& cloud : foregroundGroundClouds_) {
			if (cloud) {
				cloud->Finalize();
			}
		}
		foregroundGroundClouds_.clear();
	}

	void SceneTitle::UpdateBackgrounds() {
		for (std::unique_ptr<Atrum::Entity>& background : backgrounds_) {
			if (background) {
				background->Update();
			}
		}
	}

	void SceneTitle::DrawBackgrounds() {
		if (skyDome_) {
			skyDome_->Draw();
		}

		for (const std::unique_ptr<Atrum::Entity>& background : backgrounds_) {
			if (background) {
				background->Draw();
			}
		}

		for (const std::unique_ptr<Atrum::Entity>& cloud : foregroundGroundClouds_) {
			if (cloud) {
				cloud->Draw();
			}
		}
	}

	SceneTitle::SceneTitle() {}

	SceneTitle::~SceneTitle() = default;

	SceneTitle* SceneTitle::Instance() {
		static SceneTitle instance;
		return &instance;
	}

} // namespace Atrum
