#include "Scene/SceneTitle.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/LightData.h"
#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneGame.h"
#include "System/Entity/Entity.h"
#include "System/EntityFactory.h"
#include "Scene/SceneScreenUi.h"
#include "System/PlaySe.h"

#include <cmath>

namespace {

	void SetupStageLighting() {
		Atrum::Math::Vector3 lightDirection{ -0.4f, -0.85f, 0.15f };
		lightDirection.Normalize();
		const Atrum::DirectionalLightData directionalLight{
			.color = Atrum::Math::Vector3{ 1.0f, 1.0f, 1.0f },
			.direction = lightDirection,
			.intensity = 8.0f,
			.lightModel = Atrum::LightModel::HalfLambert,
		};
		::Atrum::EngineInstance()->SetDirectionalLightData(directionalLight);
	}

} // namespace

namespace Atrum {

	void SceneTitle::EnterScene() {
		EnsureCommonResources();
		SetupStageViewCamera();
		Atrum::SceneScreenUi::PreloadTitleUiTextures();
		SpawnBackgrounds();
		sceneBgmHandle_ = Atrum::PlayBgmLoop({ "./Assets/Audios/bgmTitle.mp3",
											  "./Resources/bgmTitle.mp3" });
		if (sceneBgmHandle_.isSuccess) {
			float pitchRatio = 1.0f;
			if (!Atrum::GetSeFrequencyRatio(sceneBgmHandle_, pitchRatio) ||
			    pitchRatio < 0.99f) {
				Atrum::SetBgmLoopPitch(sceneBgmHandle_, 1.0f);
			}
		}
		sceneAmbientLoopSeHandle_ = Atrum::PlaySeLoopCandidates(
			{"./Assets/Audios/seTitleAmbientLoop.mp3",
			 "./Resources/seTitleAmbientLoop.mp3"});
		if (sceneAmbientLoopSeHandle_.isSuccess) {
			Atrum::SetSeVolume(sceneAmbientLoopSeHandle_, 0.42f);
		}
	}

	void SceneTitle::Update() {
		Atrum::AssignPendingFadeSyncedSceneBgm(sceneBgmHandle_);
		Atrum::SyncSceneBgmForChangeSceneFade(sceneBgmHandle_);
		if (sceneBgmHandle_.isSuccess &&
		    !Atrum::CommandChangeScene::Instance()->GetIsExecute()) {
			float pitchRatio = 1.0f;
			if (Atrum::GetSeFrequencyRatio(sceneBgmHandle_, pitchRatio) &&
			    pitchRatio < 0.99f) {
				Atrum::SetBgmLoopPitch(sceneBgmHandle_, 1.0f);
			}
		}
		if (sceneAmbientLoopSeHandle_.isSuccess) {
			const float targetVolume =
				Atrum::CommandChangeScene::Instance()->GetIsExecute() ? 0.1f
				                                                      : 0.42f;
			float currentVolume = targetVolume;
			if (!Atrum::GetSeVolume(sceneAmbientLoopSeHandle_, currentVolume) ||
				std::abs(currentVolume - targetVolume) > 0.02f) {
				Atrum::SetSeVolume(sceneAmbientLoopSeHandle_, targetVolume);
			}
		}
		UpdateBackgrounds();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
		// エディタビルドの起動シーンは SceneEntityEditor（タイトル経由の遷移は無効）
		return;
#else
		const Atrum::InputData& input = Atrum::AbstractInput::Instance()->GetData();
		if (input.isSubmitTrigger) {
			Atrum::PlaySubmitSe();
			Atrum::CommandChangeScene::Instance()->Set(SceneGame::Instance(),
				std::make_unique<ChangeSceneFade>().release());
		}
#endif
	}

	void SceneTitle::DrawScene() {
		SetupStageLighting();
		DrawBackgrounds();

		if (Atrum::AtrumEngine* engine = ::Atrum::EngineInstance()) {
			Atrum::SceneScreenUi::DrawTitleSceneUi(engine, GetCamera());
		}
	}

	void SceneTitle::ExitScene() {
		ReleaseCommonResources();
		Atrum::StopBgmLoop(sceneBgmHandle_);
		sceneBgmHandle_ = {};
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
