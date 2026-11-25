#include "Scene/SceneEntityEditor.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/EditorSceneRuntimeFunctionalViewDraw.h"
#include "Scene/SceneBuildUpStageCameraFraming.h"
#include "Scene/SceneEntityEditorPreviewBuildUpParityRebuild.h"
#include "Scene/SceneEntityEditorPreviewEditorAssetPathRelink.h"
#include "Scene/SceneEntityEditorEnterSceneDocumentLoadPolicy.h"
#endif

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/LightData.h"
#include "Input/PlayInput.h"
#include "Scene/SceneBuildUpLaunchPolicy.h"
#include "Scene/SceneEntityEditorDebugHotkeys.h"
#include "Scene/SceneEntityEditorInEditorPlayTick.h"
#include "Scene/SceneEntityEditorPreviewCameraSetup.h"
#include "Scene/SceneEntityEditorPreviewStageViewCameraFallback.h"
#include "Scene/SceneEntityEditorRuntimeViewCameraDraw.h"
#include "Scene/SceneEntityEditorEditViewDebugCameraDraw.h"
#include "Scene/SceneEntityEditorEditViewTransformDrag.h"
#include "Scene/SceneEntityEditorEditViewTransformGizmoDraw.h"
#include "Scene/SceneEntityEditorSceneViewRenderCameraBind.h"
#include "Scene/SceneEntityEditorSceneViewVisualEffectsApply.h"
#include "Scene/SceneEntityEditorAuthoringLightingPreview.h"
#include "Scene/SceneEntityEditorExitSceneTeardown.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneParticleEditor.h"
#include "System/GameAssetsPaths.h"
#include "System/Particle/SceneParticleServices.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
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

void SceneEntityEditor::SetBootJsonPath(const std::string &jsonFilePath) {
  pendingBootJsonPath_ = jsonFilePath;
}

bool SceneEntityEditor::AllowDebugCameraControl() const {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  // DebugCamera の入力は Layout 後の Draw 内で行う（ホバー矩形が確定してから）
  return false;
#else
  return true;
#endif
}

const std::vector<std::unique_ptr<Entity>> *
SceneEntityEditor::GetSupplementalCameraEntities() const {
  return &previewEntities_;
}

void SceneEntityEditor::ApplyGameViewStageCameraForDraw() {
  SetupStageViewCamera();
  PrepareCamerasForDraw();
}

void SceneEntityEditor::SetupPreviewCamera() {
  cameraRig_.EnsureAutoSpawnedCameras(this);
  if (SceneEntityEditorPreviewCameraSetup::ApplyPrimaryFromPreviewEntities(
          previewEntities_, AllowDebugCameraControl())) {
    return;
  }
  if (editor_) {
    ApplyPreviewStageViewCameraFallback(*this, *editor_);
  } else {
    SetupStageViewCamera();
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
  previewEntities_ =
      RebuildPreviewEntitiesViaBuildUpParity(*editor_);
  RelinkPreviewEditorAssetPathsAfterRebuild(*editor_, previewEntities_);
  SetupPreviewCamera();
}

void SceneEntityEditor::EnterScene() {
  EnsureCommonResources();
  SceneEntityEditorAuthoringLightingPreview::ApplyOnEnterEntityEditor();
  if (editor_) {
    const SceneEntityEditorEnterSceneLoadPlan loadPlan =
        PlanEnterSceneDocumentLoad(pendingBootJsonPath_, *editor_,
                                   hasEnteredSceneEntityEditor_);
    if (!pendingBootJsonPath_.empty()) {
      editor_->Ui().jsonFilePath = pendingBootJsonPath_;
      pendingBootJsonPath_.clear();
    }
    if (loadPlan.loadFromDisk) {
      editor_->LoadFromJsonPath(loadPlan.jsonPathToLoad);
    }
    hasEnteredSceneEntityEditor_ = true;
    RebuildPreviewScene();
  } else {
    SetupPreviewCamera();
  }
}

void SceneEntityEditor::Update() {
  if (!editor_) {
    return;
  }

  if (editor_->TickPendingRequests()) {
    RebuildPreviewScene();
  }

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (EntityEditor::SceneEntityEditorInEditorPlayTick::Process(
          *editor_, [this] { RebuildPreviewScene(); })) {
    return;
  }
#endif

  if (editor_->Ui().requestLaunchSceneBuildUp) {
    editor_->Ui().requestLaunchSceneBuildUp = false;
    std::string scenePath = editor_->Ui().sceneBuildUpLaunchPath;
    editor_->Ui().sceneBuildUpLaunchPath.clear();
    if (scenePath.empty()) {
      scenePath = Atrum::Editor::Scene::TakePendingEditorPlayScenePath();
    }
    if (scenePath.empty()) {
      scenePath = editor_->Ui().jsonFilePath;
    }
    if (!scenePath.empty()) {
      editor_->SaveToJsonPath(scenePath);
      Atrum::SceneBuildUpLaunchPolicy::QueueFadeToBuildUp(scenePath,
                                                           Instance());
      return;
    }
  }

  if (editor_->Ui().requestOpenParticleEditor) {
    editor_->Ui().requestOpenParticleEditor = false;
    Atrum::CommandChangeScene::Instance()->Set(
        SceneParticleEditor::Instance(),
        std::make_unique<ChangeSceneFade>().release());
    return;
  }

  SceneEntityEditorDebugHotkeys::Tick(Instance());

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  SceneEntityEditorEditViewTransformDrag::Tick(*this);
#endif

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
    if (editor_->Ui().runtimeFunctionalViewDraw) {
      SceneBuildUpStageCamera::MaintainLockedStageCamera(*this);
    }
    Atrum::Particle::TickEmitterManagerForActiveScene(GetCamera());
  }
}

void SceneEntityEditor::Draw() {
  Atrum::Engine *engine = ::Atrum::EngineInstance();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  editor_->LayoutEditorWorkbench();
  SceneEntityEditorEditViewDebugCameraDraw::ApplyHoveredEditViewCameraInput(
      *this);
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
  // ImGui::Render の後に RT を更新。旧 SRV の解放は PostDraw 後の FlushDeferredSrvReleases
  Editor::EditorSceneRenderTargetHub::Instance().RenderSceneViews(
      engine, [this](const Editor::SceneViewTab tab, const int width,
                     const int height) {
        DrawSceneViewContent(tab, width, height);
      });
#endif

  engine->PostDraw();
  Editor::EditorSceneRenderTargetHub::Instance().FlushDeferredSrvReleases(engine);
}

void SceneEntityEditor::DrawPreviewEntities() {
  EntityEditor::DrawSceneDocumentEntities(previewEntities_);
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

  if (viewTab == Editor::SceneViewTab::Runtime) {
    const bool functionalDraw =
        editor_ != nullptr && editor_->Ui().runtimeFunctionalViewDraw;
    SceneEntityEditorRuntimeViewCameraDraw::PrepareSceneViewCameraForDraw(
        *this, viewTab, previewEntities_, functionalDraw);
  }

  Atrum::Camera *camera = GetCamera();
  if (camera == nullptr) {
    return;
  }

#if defined(DEVELOPMENT) && !defined(NDEBUG)
  auto *debugCamera = dynamic_cast<Atrum::Debug::DebugCamera *>(camera);
  SavedDebugCameraState savedCamera{};
  if (viewTab == Editor::SceneViewTab::Game) {
    if (debugCamera != nullptr) {
      savedCamera = CaptureDebugCamera(debugCamera);
    }
    ApplyGameViewStageCameraForDraw();
    camera = GetCamera();
    debugCamera = dynamic_cast<Atrum::Debug::DebugCamera *>(camera);
  }
#elif defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (viewTab == Editor::SceneViewTab::Game) {
    ApplyGameViewStageCameraForDraw();
    camera = GetCamera();
  }
#endif

  if (!isEditViewPass && editor_ != nullptr &&
      editor_->Ui().runtimeFunctionalViewDraw) {
#if defined(DEVELOPMENT) && !defined(NDEBUG)
    if (viewTab == Editor::SceneViewTab::Game && debugCamera != nullptr) {
      RestoreDebugCamera(debugCamera, savedCamera);
      debugCamera = nullptr;
    }
#endif
    Editor::DrawEditorSceneRuntimeFunctionalView(*this, editor_->SceneDocument(),
                                                previewEntities_, width, height);
    return;
  }

  SceneEntityEditorSceneViewRenderCameraBind::BindForSceneViewRender(
      engine, camera, width, height);
  SceneEntityEditorSceneViewVisualEffectsApply::ApplyBeforeDraw(engine);

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);
  if (isEditViewPass) {
    Editor::DrawInfiniteEditorFloorGrid(engine, camera, width, height);
  }
#endif

  DrawPreviewEntities();
  if (isEditViewPass && editor_ != nullptr) {
    SceneEntityEditorEditViewTransformGizmoDraw::Draw(engine, *this);
  }
  if (!isEditViewPass) {
    Atrum::Particle::DrawEmitterManagerForActiveScene();
  }

#if defined(DEVELOPMENT) && !defined(NDEBUG)
  if (viewTab == Editor::SceneViewTab::Game && debugCamera != nullptr) {
    RestoreDebugCamera(debugCamera, savedCamera);
  }
#endif
}

#endif

void SceneEntityEditor::DrawScene() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  // Draw() 内でレンダーテクスチャへ描画済み
#else
  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }
  engine->SetCamera(GetCamera());
  SceneEntityEditorSceneViewVisualEffectsApply::ApplyBeforeDraw(engine);
  DrawPreviewEntities();
#endif
}

void SceneEntityEditor::ExitScene() {
  Atrum::Editor::Scene::ClearStagedEditorPlayScenePath();
  EntityEditor::SceneEntityEditorExitSceneTeardown::ReleasePreviewEntities(
      previewEntities_);
  if (editor_) {
    EntityEditor::SceneEntityEditorExitSceneTeardown::ResetEditorTransientPlayUi(
        *editor_);
  }
  ReleaseCommonResources();
}

} // namespace Atrum
