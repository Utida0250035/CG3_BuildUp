#include "Scene/SceneParticleEditor.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/LightData.h"
#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneEntityEditor.h"
#endif
#include "System/Json/JsonHandler.h"
#include "System/Particle/EmitterManager.h"
#include "System/Particle/ParticleEmitter.h"
#include "Input/PlayInput.h"

#include "System/Editor/EditorAssetsPaths.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/Particle/ParticleEditorReferenceGrid.h"
#include "System/Editor/Particle/ParticleEditorUi.h"
#endif

#include <filesystem>
#include <stdexcept>

namespace {

	/// EmitterManager に登録するプレビュー用の固定名（戦闘用プリフィックスと衝突しない）
	constexpr const char *kParticleEditorPreviewEmitterName =
	    "particle_editor_preview";

	void SetupEditorLighting() {
		Atrum::Math::Vector3 lightDirection{ -0.35f, -0.9f, 0.2f };
		lightDirection.Normalize();
		const Atrum::DirectionalLightData directionalLight{
			.color = Atrum::Math::Vector3{ 1.0f, 1.0f, 1.0f },
			.direction = lightDirection,
			.intensity = 6.0f,
			.lightModel = Atrum::LightModel::HalfLambert,
		};
		::Atrum::EngineInstance()->SetDirectionalLightData(directionalLight);
	}

	Atrum::Particle::Emitter::EmitterManager *EditorEmitterManager() {
		Atrum::Scene *sceneNow = Atrum::CommandScene::Instance()->GetSceneNow();
		return sceneNow ? sceneNow->GetParticleEmitterManager() : nullptr;
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

	Atrum::Particle::Emitter::EmitterManager *manager = EditorEmitterManager();
	if (!manager) {
		return;
	}
	manager->RemoveByName(kParticleEditorPreviewEmitterName);

	auto emitter = std::make_unique<Atrum::Visual::Particle::Emitter>();
	emitter->SetName(kParticleEditorPreviewEmitterName);
	emitter->SetCamera(GetCamera());
	emitter->RebuildFromParams(layerParams_);

	manager->Register(kParticleEditorPreviewEmitterName, std::move(emitter));
	manager->SetRetainInactiveByName(kParticleEditorPreviewEmitterName, true);
}

void SceneParticleEditor::SaveToJsonPath(const std::string& jsonFilePath) {
	Atrum::Json::JsonHandler::SaveToFile(jsonFilePath, layerParams_);
}

void SceneParticleEditor::ApplyPreview() {
	Atrum::Particle::Emitter::EmitterManager *manager = EditorEmitterManager();
	if (!manager) {
		return;
	}
	manager->RemoveByName(kParticleEditorPreviewEmitterName);

	auto emitter = std::make_unique<Atrum::Visual::Particle::Emitter>();
	emitter->SetName(kParticleEditorPreviewEmitterName);
	emitter->SetCamera(GetCamera());
	emitter->RebuildFromParams(layerParams_);

	manager->Register(kParticleEditorPreviewEmitterName, std::move(emitter));
	manager->SetRetainInactiveByName(kParticleEditorPreviewEmitterName, true);
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
		if (Atrum::Particle::Emitter::EmitterManager *mgr = EditorEmitterManager()) {
			if (Atrum::Visual::Particle::Emitter *emitter =
			        mgr->Find(kParticleEditorPreviewEmitterName)) {
				emitter->RestartPreview();
			}
		}
	}
#endif
}

void SceneParticleEditor::EnterScene() {
	EnsureCommonResources();
	SetupPreviewCamera();
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	if (uiState_) {
		LoadFromJsonPath(uiState_->jsonFilePath);
	} else {
		LoadFromJsonPath(
		    std::string(Atrum::EditorAssets::kParticlesDirectory) +
		    "/editorTest.json");
	}
#else
	LoadFromJsonPath(std::string(Atrum::EditorAssets::kParticlesDirectory) +
	                 "/editorTest.json");
#endif
}

void SceneParticleEditor::Update() {
	HandleEditorRequests();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F1)) {
		Atrum::CommandChangeScene::Instance()->Set(SceneEntityEditor::Instance(),
			std::make_unique<ChangeSceneFade>().release());
		return;
	}
#endif

	Atrum::Particle::Emitter::EmitterManager *particleManager =
	    EditorEmitterManager();
	if (!particleManager) {
		return;
	}
	particleManager->SetCamera(GetCamera());

	if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::SPACE)) {
		if (Atrum::Visual::Particle::Emitter *emitter =
		        particleManager->Find(kParticleEditorPreviewEmitterName)) {
			emitter->RestartPreview();
		}
	}

	if (particleManager->Find(kParticleEditorPreviewEmitterName) != nullptr) {
		particleManager->Update();
	}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	if (uiState_) {
		Atrum::ParticleEditor::Draw(*uiState_, layerParams_);
	}
#endif
}

void SceneParticleEditor::DrawScene() {
	SetupEditorLighting();

	Atrum::Engine* engine = ::Atrum::EngineInstance();
	Atrum::Particle::Emitter::EmitterManager *mgr = EditorEmitterManager();
	if (engine == nullptr || mgr == nullptr ||
	    mgr->Find(kParticleEditorPreviewEmitterName) == nullptr) {
		return;
	}

	engine->SetCamera(GetCamera());

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
	engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);
	DrawParticleEditorReferenceGrid(engine);
#endif

	engine->SetBlendModeForFrame(Atrum::BlendMode::ADD);
	mgr->Draw();
	engine->SetBlendModeForFrame(Atrum::BlendMode::NONE);
}

void SceneParticleEditor::ExitScene() {
	if (Atrum::Particle::Emitter::EmitterManager *mgr = EditorEmitterManager()) {
		mgr->RemoveByName(kParticleEditorPreviewEmitterName);
	}
	ReleaseCommonResources();
	layerParams_.clear();
}

} // namespace Atrum
