#include "Scene/SceneEntityEditor.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/LightData.h"
#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneTitle.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Particle/ParticleEditorReferenceGrid.h"
#include "System/Entity/Entity.h"

namespace {

void SetupEditorLighting() {
  Atrum::Math::Vector3 lightDirection{-0.35f, -0.9f, 0.2f};
  lightDirection.Normalize();
  const Atrum::DirectionalLightData directionalLight{
      .color = Atrum::Math::Vector3{1.0f, 1.0f, 1.0f},
      .direction = lightDirection,
      .intensity = 6.0f,
      .lightModel = Atrum::LightModel::HalfLambert,
  };
  Atrum::Engine::Instance()->SetDirectionalLightData(directionalLight);
}

} // namespace

namespace Atrum {

SceneEntityEditor::SceneEntityEditor() {
  editor_ = std::make_unique<EntityEditor::EntityEditor>();
}

SceneEntityEditor::~SceneEntityEditor() = default;

SceneEntityEditor *SceneEntityEditor::Instance() {
  static SceneEntityEditor instance;
  return &instance;
}

void SceneEntityEditor::SetupPreviewCamera() {
  if (Atrum::Camera *camera = GetCamera()) {
    camera->SetTranslate(Atrum::Math::Vector3{0.0f, 1.6f, -6.0f});
    camera->UpdateMatrix();
  }
}

void SceneEntityEditor::RebuildPreviewEntity() {
  if (previewEntity_) {
    previewEntity_->Finalize();
    previewEntity_.reset();
  }
  if (!editor_) {
    return;
  }
  previewEntity_ = editor_->BuildPreviewEntity();
}

void SceneEntityEditor::EnterScene() {
  EnsureCommonResources();
  SetupPreviewCamera();
  if (editor_) {
    editor_->LoadFromJsonPath(editor_->Ui().jsonFilePath);
    RebuildPreviewEntity();
  }
}

void SceneEntityEditor::Update() {
  if (!editor_) {
    return;
  }

  if (editor_->TickPendingRequests()) {
    RebuildPreviewEntity();
  }

  if (Atrum::Input::PlayInput::Instance()->IsKeyTrigger(Atrum::Input::Key::F1)) {
    Atrum::CommandChangeScene::Instance()->Set(SceneTitle::Instance(),
                                               std::make_unique<ChangeSceneFade>().release());
    return;
  }

  if (previewEntity_) {
    previewEntity_->Update();
  }

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  editor_->DrawEditorUi();
#endif
}

void SceneEntityEditor::DrawScene() {
  SetupEditorLighting();

  Atrum::Engine *engine = Atrum::Engine::Instance();
  if (engine == nullptr || !previewEntity_) {
    return;
  }

  engine->SetCamera(GetCamera());

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);
  DrawParticleEditorReferenceGrid(engine);
#endif

  previewEntity_->Draw();
}

void SceneEntityEditor::ExitScene() {
  if (previewEntity_) {
    previewEntity_->Finalize();
    previewEntity_.reset();
  }
  ReleaseCommonResources();
}

} // namespace Atrum
