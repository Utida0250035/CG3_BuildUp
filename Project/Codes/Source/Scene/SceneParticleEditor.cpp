#include "Scene/SceneParticleEditor.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/LightData.h"
#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneTitle.h"
#include "System/Json/JsonHandler.h"
#include "System/Particle/ParticleEmitter.h"
#include "Input/PlayInput.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/Particle/ParticleEditorUi.h"
#endif

#include <filesystem>
#include <stdexcept>

namespace {

	void SetupEditorLighting() {
		Atrum::Math::Vector3 lightDirection{ -0.35f, -0.9f, 0.2f };
		lightDirection.Normalize();
		const Atrum::DirectionalLightData directionalLight{
			.color = Atrum::Math::Vector3{ 1.0f, 1.0f, 1.0f },
			.direction = lightDirection,
			.intensity = 6.0f,
			.lightModel = Atrum::LightModel::HalfLambert,
		};
		Atrum::Engine::Instance()->SetDirectionalLightData(directionalLight);
	}

} // namespace

namespace Atrum {

SceneParticleEditor::SceneParticleEditor() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	uiState_ = std::make_unique<Atrum::ParticleEditor::UiState>();
#endif
}

SceneParticleEditor::~SceneParticleEditor() = default;

SceneParticleEditor* SceneParticleEditor::Instance() {
	static SceneParticleEditor instance;
	return &instance;
}

void SceneParticleEditor::SetupPreviewCamera() {
	if (Atrum::Camera* camera = GetCamera()) {
		camera->SetTranslate(Atrum::Math::Vector3{ 0.0f, 1.6f, -6.0f });
		camera->UpdateMatrix();
	}
}

void SceneParticleEditor::LoadFromJsonPath(const std::string& jsonFilePath) {
	namespace fs = std::filesystem;
	if (!fs::exists(jsonFilePath)) {
		layerParams_ = { Atrum::Visual::Particle::Param{} };
	} else {
		layerParams_ =
			Atrum::Json::JsonHandler::LoadFromFile<
			std::vector<Atrum::Visual::Particle::Param>>(jsonFilePath);
	}

	if (!previewEmitter_) {
		previewEmitter_ = std::make_unique<Atrum::Visual::Particle::Emitter>();
	}
	previewEmitter_->SetName("particle_editor_preview");
	previewEmitter_->SetCamera(GetCamera());
	previewEmitter_->RebuildFromParams(layerParams_);
}

void SceneParticleEditor::SaveToJsonPath(const std::string& jsonFilePath) {
	Atrum::Json::JsonHandler::SaveToFile(jsonFilePath, layerParams_);
}

void SceneParticleEditor::ApplyPreview() {
	if (!previewEmitter_) {
		previewEmitter_ = std::make_unique<Atrum::Visual::Particle::Emitter>();
	}
	previewEmitter_->SetCamera(GetCamera());
	previewEmitter_->RebuildFromParams(layerParams_);
}

void SceneParticleEditor::HandleEditorRequests() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	if (!uiState_) {
		return;
	}

	if (uiState_->requestReloadFromDisk) {
		uiState_->requestReloadFromDisk = false;
		LoadFromJsonPath(uiState_->jsonFilePath);
	}
	if (uiState_->requestSaveToDisk) {
		uiState_->requestSaveToDisk = false;
		try {
			SaveToJsonPath(uiState_->jsonFilePath);
		} catch (const std::exception&) {
		  // 保存失敗時は次フレームで再試行できるよう何もしない
		}
	}
	if (uiState_->requestApplyPreview) {
		uiState_->requestApplyPreview = false;
		ApplyPreview();
	}
	if (uiState_->requestReplay) {
		uiState_->requestReplay = false;
		if (previewEmitter_) {
			previewEmitter_->RestartPreview();
		}
	}
#endif
}

void SceneParticleEditor::EnterScene() {
	SetupPreviewCamera();
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	if (uiState_) {
		LoadFromJsonPath(uiState_->jsonFilePath);
	} else {
		LoadFromJsonPath("../EditorAssets/Particles/editorTest.json");
	}
#else
	LoadFromJsonPath("../EditorAssets/Particles/editorTest.json");
#endif
}

void SceneParticleEditor::Update() {
	HandleEditorRequests();

	if (Atrum::Input::PlayInput::Instance()->IsKeyTrigger(Atrum::Input::Key::F1)) {
		Atrum::CommandChangeScene::Instance()->Set(SceneTitle::Instance(),
			new Atrum::ChangeSceneFade());
		return;
	}

	if (Atrum::Input::PlayInput::Instance()->IsKeyTrigger(Atrum::Input::Key::SPACE) && previewEmitter_) {
		previewEmitter_->RestartPreview();
	}

	if (previewEmitter_) {
		previewEmitter_->SetCamera(GetCamera());
		previewEmitter_->Update();
	}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	if (uiState_) {
		Atrum::ParticleEditor::Draw(*uiState_, layerParams_);
	}
#endif
}

void SceneParticleEditor::DrawScene() {
	SetupEditorLighting();

	Atrum::Engine* engine = Atrum::Engine::Instance();
	if (engine == nullptr || previewEmitter_ == nullptr) {
		return;
	}

	engine->SetCamera(GetCamera());
	engine->SetBlendModeForFrame(Atrum::BlendMode::ADD);
	previewEmitter_->Draw();
	engine->SetBlendModeForFrame(Atrum::BlendMode::NONE);
}

void SceneParticleEditor::ExitScene() {
	previewEmitter_.reset();
	layerParams_.clear();
}

} // namespace Atrum
