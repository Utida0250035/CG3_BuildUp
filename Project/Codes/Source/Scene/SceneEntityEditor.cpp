#include "Scene/SceneEntityEditor.h"

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/LightData.h"
#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneParticleEditor.h"
#include "System/GameAssetsPaths.h"
#include "System/Particle/SceneParticleServices.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Entity/Entity.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/EditorFrame.h"
#include "System/Editor/EditorInfiniteGrid.h"
#include "System/Editor/EditorSceneRenderTarget.h"
#endif
#include <vector>

#if defined(DEVELOPMENT) && !defined(NDEBUG)
#include "ForDebug/DebugCamera.h"
#endif

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
  ::Atrum::EngineInstance()->SetDirectionalLightData(directionalLight);
}

#if defined(DEVELOPMENT) && !defined(NDEBUG)

struct SavedDebugCameraState {
  Atrum::Math::Vector3 translate{};
  Atrum::Math::Quaternion quaternion{Atrum::Math::Quaternion::Identity()};
  Atrum::Math::Matrix4x4 perspective{};
};

SavedDebugCameraState CaptureDebugCamera(Atrum::Debug::DebugCamera *camera) {
  SavedDebugCameraState state{};
  if (camera == nullptr) {
    return state;
  }
  state.translate = camera->RefTranslate();
  state.quaternion = camera->RefQuaternion();
  state.perspective = camera->GetPerspectiveFovMatrix();
  return state;
}

void RestoreDebugCamera(Atrum::Debug::DebugCamera *camera,
                        const SavedDebugCameraState &state) {
  if (camera == nullptr) {
    return;
  }
  camera->RefTranslate() = state.translate;
  camera->RefQuaternion() = state.quaternion;
  camera->SetPerspectiveFovMatrix(state.perspective);
  camera->UpdateMatrix();
}

#endif

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

bool SceneEntityEditor::AllowDebugCameraControl() const {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  // DebugCamera の入力は Layout 後の Draw 内で行う（ホバー矩形が確定してから）
  return false;
#else
  return true;
#endif
}

void SceneEntityEditor::SetupPreviewCamera() {
  if (Atrum::Camera *camera = GetCamera()) {
    camera->SetTranslate(Atrum::Math::Vector3{0.0f, 1.6f, -6.0f});
    camera->UpdateMatrix();
  }
}

void SceneEntityEditor::RebuildPreviewScene() {
  for (std::unique_ptr<Entity> &entity : previewEntities_) {
    if (entity) {
      entity->Finalize();
    }
  }
  previewEntities_.clear();
  if (!editor_) {
    return;
  }
  previewEntities_ = editor_->BuildPreviewScene();
}

void SceneEntityEditor::EnterScene() {
  EnsureCommonResources();
  SetupPreviewCamera();
  if (editor_) {
    editor_->LoadFromJsonPath(editor_->Ui().jsonFilePath);
    RebuildPreviewScene();
  }
}

void SceneEntityEditor::Update() {
  if (!editor_) {
    return;
  }

  if (editor_->TickPendingRequests()) {
    RebuildPreviewScene();
  }

  if (editor_->Ui().requestPlayRuntimeScene) {
    editor_->Ui().requestPlayRuntimeScene = false;
    editor_->SaveToJsonPath(editor_->Ui().jsonFilePath);
    RebuildPreviewScene();
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
    Editor::EditorFrame::Instance().SetSceneViewTab(
        Editor::SceneViewTab::Runtime);
    editor_->Ui().previewSimulationEnabled = true;
    editor_->Ui().requestFocusRuntimeSceneView = true;
#endif
  }

  if (editor_->Ui().requestOpenParticleEditor) {
    editor_->Ui().requestOpenParticleEditor = false;
    Atrum::CommandChangeScene::Instance()->Set(
        SceneParticleEditor::Instance(),
        std::make_unique<ChangeSceneFade>().release());
    return;
  }

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F2)) {
    Atrum::CommandChangeScene::Instance()->Set(
        SceneParticleEditor::Instance(),
        std::make_unique<ChangeSceneFade>().release());
    return;
  }

  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F3)) {
    const std::string demoScene =
        std::string(Atrum::GameAssets::kScenesDirectory) + "/ParticleDemo.json";
    Atrum::CommandChangeScene::Instance()->Set(
        SceneBuildUpRegistry::Instance().Launch(demoScene, Instance()),
        std::make_unique<ChangeSceneFade>().release());
    return;
  }

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  const Editor::SceneViewTab viewTab =
      Editor::EditorFrame::Instance().SceneViewTabSelection();
  const bool simulateRuntimeTab = viewTab == Editor::SceneViewTab::Runtime;
  const bool simulateEditPlay =
      viewTab == Editor::SceneViewTab::Edit &&
      editor_->Ui().previewSimulationEnabled;
#else
  const bool simulateRuntimeTab = false;
  const bool simulateEditPlay = editor_->Ui().previewSimulationEnabled;
#endif
  if (simulateRuntimeTab || simulateEditPlay) {
    for (std::unique_ptr<Entity> &entity : previewEntities_) {
      if (entity && entity->IsActive()) {
        entity->Update();
      }
    }
    Atrum::Particle::TickEmitterManagerForActiveScene(GetCamera());
  }
}

void SceneEntityEditor::Draw() {
  Atrum::Engine *engine = ::Atrum::EngineInstance();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  editor_->LayoutEditorWorkbench();
  // エディットビュー上だけ既存 DebugCamera で操作（ImGui::Image ホバー後）
  if (Editor::EditorFrame::Instance().EditViewHovered()) {
    if (Atrum::Camera *camera = GetCamera()) {
      camera->Update();
    }
  }
#endif

  engine->PreDraw();

#if !defined(DEVELOPMENT) || !defined(USE_IMGUI)
  DrawScene();
#endif

  Atrum::CommandChangeScene::Instance()->Draw();
#ifdef USE_IMGUI
  engine->ImGuiRender();
#endif

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  // ImGui::Render の後に RT を更新（レイアウト済みディスクリプタを同一フレームで破棄しない）
  Editor::EditorSceneRenderTargetHub::Instance().RenderSceneViews(
      engine, [this](const Editor::SceneViewTab tab, const int width,
                     const int height) {
        DrawSceneViewContent(tab, width, height);
      });
#endif

  engine->PostDraw();
}

void SceneEntityEditor::DrawPreviewEntities() {
  for (std::unique_ptr<Entity> &entity : previewEntities_) {
    if (!entity) {
      continue;
    }
    if (entity->GetParent() == nullptr) {
      entity->Draw();
    }
  }
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

void SceneEntityEditor::DrawSceneViewContent(
    const Editor::SceneViewTab viewTab, const int width, const int height) {
  const bool isEditViewPass = viewTab == Editor::SceneViewTab::Edit;
  if (width <= 0 || height <= 0) {
    return;
  }

  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  Atrum::Camera *camera = GetCamera();
  if (camera == nullptr) {
    return;
  }

#if defined(DEVELOPMENT) && !defined(NDEBUG)
  auto *debugCamera = dynamic_cast<Atrum::Debug::DebugCamera *>(camera);
  SavedDebugCameraState savedCamera{};
  if (!isEditViewPass && debugCamera != nullptr) {
    savedCamera = CaptureDebugCamera(debugCamera);
    SetupStageViewCamera();
    camera = GetCamera();
    debugCamera = dynamic_cast<Atrum::Debug::DebugCamera *>(camera);
  }
#endif

  Editor::FitCameraPerspectiveToViewport(camera, width, height);
  engine->SetCamera(camera);
  SetupEditorLighting();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);
  if (isEditViewPass) {
    Editor::DrawInfiniteEditorFloorGrid(engine, camera, width, height);
  }
#endif

  DrawPreviewEntities();
  if (!isEditViewPass) {
    Atrum::Particle::DrawEmitterManagerForActiveScene();
  }

#if defined(DEVELOPMENT) && !defined(NDEBUG)
  if (!isEditViewPass && debugCamera != nullptr) {
    RestoreDebugCamera(debugCamera, savedCamera);
  }
#endif
}

#endif

void SceneEntityEditor::DrawScene() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  // Draw() 内でレンダーテクスチャへ描画済み
#else
  SetupEditorLighting();
  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }
  engine->SetCamera(GetCamera());
  DrawPreviewEntities();
#endif
}

void SceneEntityEditor::ExitScene() {
  for (std::unique_ptr<Entity> &entity : previewEntities_) {
    if (entity) {
      entity->Finalize();
    }
  }
  previewEntities_.clear();
  ReleaseCommonResources();
}

} // namespace Atrum
