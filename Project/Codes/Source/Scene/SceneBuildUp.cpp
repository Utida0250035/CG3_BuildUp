#include "Scene/SceneBuildUp.h"

#include "Camera/PlayerDamageScreenVignette.h"
#include "Scene/SceneBuildUpPlayerSilhouetteDraw.h"
#include "Scene/SceneBuildUpSceneJsonPathResolve.h"
#include "Scene/SceneBuildUpSceneDisplayNameResolve.h"
#include "Scene/SceneBuildUpRegistryLaunchDedupe.h"
#include "Scene/SceneBuildUpAudioPolicy.h"
#include "Scene/SceneBuildUpSceneLoadFailure.h"
#include "Scene/SceneBuildUpSceneDocumentLoad.h"
#include "Scene/SceneBuildUpLoadFailureEntities.h"
#include "Scene/SceneBuildUpRuntimeSubviewDrawCamera.h"
#include "Scene/SceneBuildUpStageCameraLock.h"
#include "Scene/SceneBuildUpF1ReturnRequest.h"
#include "Scene/SceneBuildUpReturnTarget.h"
#include "Scene/SceneBuildUpStageCameraFraming.h"
#include "Scene/SceneBuildUpExitSceneEditorUiRestore.h"
#include "Scene/SceneBuildUpRuntimeLightingPolicy.h"
#include "Scene/SceneBuildUpEnterSceneEditorUiPolicy.h"
#include "Scene/SceneBuildUpMissingPrimaryCameraFeedback.h"
#include "Scene/SceneBuildUpChangeSceneDrawPolicy.h"
#include "Scene/SceneBuildUpRuntimeEntityDraw.h"
#include "Scene/SceneBuildUpAtomFFloorSpawn.h"
#include "Scene/SceneBuildUpAtomFPlayerSpawn.h"
#include "Scene/SceneBuildUpAtomFEnemySpawn.h"
#include "Scene/SceneBuildUpAtomFEntityDestroyPurge.h"
#include "System/AtomF/AtomFScenePath.h"
#include "System/AtomF/AtomFScenePlayerLocator.h"
#include "Scene/SceneBuildUpAtomFArenaContainmentTick.h"
#include "Scene/SceneBuildUpAtomFCombatCamera.h"
#include "Scene/SceneBuildUpAtomFParametersBoot.h"
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneBuildUpDedicatedRuntimeViewWindow.h"
#endif
#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Blend/BlendMode.h"
#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "System/DeltaTime.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/Entity/Entity.h"
#include "System/Particle/SceneParticleServices.h"

#include <algorithm>

#if !defined(DEVELOPMENT) && !defined(USE_IMGUI)
#include "Scene/SceneReleaseCarousel.h"
#include "Scene/SceneScreenUi.h"
#endif

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#endif

namespace Atrum {

SceneBuildUp::SceneBuildUp(std::string sceneJsonPath, Scene *returnScene)
    : sceneJsonPath_(std::move(sceneJsonPath)), returnScene_(returnScene) {
  sceneDisplayName_ = SceneBuildUpSceneDisplayNameResolve::ResolveInitialSceneDisplayName(
      sceneJsonPath_);
}

SceneBuildUp::~SceneBuildUp() = default;

bool SceneBuildUp::IsStageViewCameraLocked() const {
  return stageViewCameraLocked_;
}

bool SceneBuildUp::TryHandleInputWhileSceneTransitionBlocked() {
  return SceneBuildUpF1ReturnRequest::TryRequestReturnViaF1(*this);
}

SceneBuildUpRegistry::~SceneBuildUpRegistry() = default;

SceneBuildUpRegistry &SceneBuildUpRegistry::Instance() {
  static SceneBuildUpRegistry instance;
  return instance;
}

Scene *SceneBuildUpRegistry::Launch(const std::string &sceneJsonPath,
                                    Scene *returnScene) {
  const std::string resolvedPath =
      ResolveSceneBuildUpJsonPath(sceneJsonPath);
  return SceneBuildUpRegistryLaunchDedupe::AcquireOrCreateOwned(
      ownedScenes_, resolvedPath, returnScene);
}

void SceneBuildUpRegistry::OnSceneExited(SceneBuildUp *scene) {
  if (scene == nullptr) {
    return;
  }
  ownedScenes_.erase(
      std::remove_if(ownedScenes_.begin(), ownedScenes_.end(),
                     [scene](const std::unique_ptr<SceneBuildUp> &owned) {
                       return owned.get() == scene;
                     }),
      ownedScenes_.end());
}

void SceneBuildUp::TeardownEntities() {
  for (std::unique_ptr<Entity> &entity : entities_) {
    if (entity) {
      entity->Finalize();
    }
  }
  entities_.clear();
}

void SceneBuildUp::DrawRuntimeEntities() {
  SceneBuildUpRuntimeEntityDraw::Draw(entities_);
}

void SceneBuildUp::EnterScene() {
  EnsureCommonResources();
  TeardownEntities();
  PlayerDamageScreenVignette::Instance().Reset();

  sceneDocumentLoadFailed_ = false;
  sceneLoadFailureMessage_.clear();
  stageViewCameraLocked_ = SceneBuildUpStageCameraLock::kDefaultLocked;
  SyncAtomFParametersWithScenePath(sceneJsonPath_);

  EntityEditor::EditorSceneDocument document{};
  const bool loadSucceeded =
      SceneBuildUpSceneDocumentLoad::TryLoad(sceneJsonPath_, document);
  if (!loadSucceeded) {
    sceneDocumentLoadFailed_ = true;
    sceneLoadFailureMessage_ =
        BuildSceneBuildUpSceneLoadFailureMessage(sceneJsonPath_);
    document = {};
    document.sceneName = sceneDisplayName_;
  }

  if (!document.sceneName.empty()) {
    sceneDisplayName_ = document.sceneName;
  }

  SceneBuildUpStageCamera::ApplyFramingFromSceneDocument(*this, document);
  entities_ = SceneBuildUpLoadFailureEntities::BuildEntitiesAfterDocumentLoad(
      loadSucceeded, document);
  AppendAtomFFloorIfScene(sceneJsonPath_, entities_);
  AppendAtomFPlayerIfScene(sceneJsonPath_, entities_);
  AppendAtomFEnemyIfScene(sceneJsonPath_, entities_);
  if (SceneBuildUpAtomFCombatCamera::ApplyOnEnterIfAtomFScene(*this)) {
    // atomF 用カメラを適用済み
  } else {
    SceneBuildUpStageCamera::ApplyStageCameraAfterSceneLoad(*this);
  }
  SceneBuildUpRuntimeLightingPolicy::ApplyOnceOnEnterScene();
  SceneBuildUpAudioPolicy::OnEnterScene(*this);
  SceneBuildUpEnterSceneEditorUiPolicy::ApplyOnEnterScene();

#if !defined(DEVELOPMENT) && !defined(USE_IMGUI)
  showsNextSceneHint_ =
      SceneFlow::SceneReleaseCarousel::Instance().HasBuildUpEntry(
          sceneJsonPath_);
  if (showsNextSceneHint_) {
    SceneScreenUi::PreloadReleaseCarouselUiTextures();
  }
#endif

}

void SceneBuildUp::Update() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (SceneBuildUpStageCameraLock::TryToggleLockHotkey(*this)) {
    return;
  }
#endif
  if (SceneBuildUpF1ReturnRequest::TryRequestReturnViaF1(*this)) {
    return;
  }

  if (!sceneDocumentLoadFailed_) {
    if (AtomF::PathEndsWithAtomFJson(sceneJsonPath_)) {
      AtomF::RefreshScenePlayerLocatorFromEntities(entities_);
    }
    for (std::unique_ptr<Entity> &entity : entities_) {
      if (entity && entity->IsActive()) {
        entity->Update();
      }
    }

    PurgeDestroyedSceneEntities(entities_);

    TickAtomFArenaContainmentForScene(sceneJsonPath_, entities_);

    TickCameraRigEntities();

    Particle::TickEmitterManagerForActiveScene(GetCamera());

    if (FrameDeltaTime *const frameDelta = ::Atrum::EngineFrameDeltaTime()) {
      PlayerDamageScreenVignette::Instance().Update(frameDelta->GetDeltaTime());
    }
  }
  SceneBuildUpAtomFCombatCamera::MaintainSceneCamera(*this);
  SceneBuildUpAudioPolicy::Tick(*this);
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

void SceneBuildUp::DrawRuntimeViewContent(const int width, const int height) {
  if (width <= 0 || height <= 0) {
    return;
  }

  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  if (sceneDocumentLoadFailed_) {
    DrawSceneBuildUpSceneLoadFailureDimOverlay(engine);
    return;
  }

  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);

  Atrum::Camera *const camera =
      SceneBuildUpRuntimeSubviewDrawCamera::BindAfterGameOrderPrepare(
          *this, width, height);
  if (camera == nullptr) {
    HandleSceneBuildUpDrawWithoutPrimaryCamera(engine, sceneJsonPath_);
    return;
  }
  engine->SetCamera(camera);
  DrawRuntimeEntities();
  Particle::DrawEmitterManagerForActiveScene();
  SceneBuildUpPlayerSilhouette::DrawAfterRuntimeEntities(engine, entities_);
  engine->SetCamera(camera);
  PlayerDamageScreenVignette::Instance().Draw(engine);
}

#endif

void SceneBuildUp::DrawScene() {
  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  if (sceneDocumentLoadFailed_) {
    DrawSceneBuildUpSceneLoadFailureDimOverlay(engine);
    return;
  }

  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);
  SceneBuildUpAtomFCombatCamera::MaintainSceneCamera(*this);
  Atrum::Camera *const primaryCamera = GetCamera();
  if (primaryCamera == nullptr) {
    HandleSceneBuildUpDrawWithoutPrimaryCamera(engine, sceneJsonPath_);
    return;
  }
  engine->SetCamera(primaryCamera);
  DrawRuntimeEntities();
  Particle::DrawEmitterManagerForActiveScene();
  SceneBuildUpPlayerSilhouette::DrawAfterRuntimeEntities(engine, entities_);
  engine->SetCamera(primaryCamera);
  PlayerDamageScreenVignette::Instance().Draw(engine);

#if !defined(DEVELOPMENT) && !defined(USE_IMGUI)
  if (showsNextSceneHint_) {
    SceneScreenUi::DrawReleaseCarouselHowToNextSceneUi(engine, GetCamera());
  }
#endif
}

void SceneBuildUp::Draw() {
  Atrum::Engine *engine = ::Atrum::EngineInstance();

  engine->PreDraw();
  DrawScene();
  SceneBuildUpChangeSceneDrawPolicy::DrawTransitionWhenActive();
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (sceneDocumentLoadFailed_ && !sceneLoadFailureMessage_.empty()) {
    DrawSceneBuildUpSceneLoadFailureNotice(sceneLoadFailureMessage_);
  } else {
    DrawSceneBuildUpMissingPrimaryCameraNoticeIfPending();
  }
#endif
#ifdef USE_IMGUI
  engine->ImGuiRender();
#endif
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  SceneBuildUpDedicatedRuntimeViewWindow::LayoutAndRenderViews(*this);
#endif
  engine->PostDraw();
}

void SceneBuildUp::ExitScene() {
  PlayerDamageScreenVignette::Instance().Reset();
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  Editor::Scene::ClearStagedEditorPlayScenePath();
  SceneBuildUpExitSceneEditorUiRestore::ApplyOnExitScene();
#endif
  SceneBuildUpAudioPolicy::OnExitScene(*this);
  TeardownEntities();
  ReleaseCommonResources();
  SceneBuildUpRegistry::Instance().OnSceneExited(this);
}

} // namespace Atrum
