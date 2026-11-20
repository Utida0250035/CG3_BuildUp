#include "Scene/SceneBuildUp.h"

#include "Scene/SceneBuildUpPlayerSilhouetteDraw.h"
#include "Scene/SceneBuildUpRuntimeVisualApply.h"
#include "Scene/SceneBuildUpSceneJsonPathResolve.h"
#include "Scene/SceneBuildUpRegistryLaunchDedupe.h"
#include "Scene/SceneBuildUpAudioPolicy.h"
#include "Scene/SceneBuildUpSceneLoadFailure.h"
#include "Scene/SceneBuildUpSceneDocumentLoad.h"
#include "Scene/SceneBuildUpLoadFailureEntities.h"
#include "Scene/SceneBuildUpReturnTarget.h"
#include "Scene/SceneBuildUpStageCameraFraming.h"
#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "Engine/Blend/BlendMode.h"
#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
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
  sceneDisplayName_ =
      Editor::Scene::SceneDisplayNameFromPath(sceneJsonPath_);
}

SceneBuildUp::~SceneBuildUp() = default;

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

void SceneBuildUp::SetupRuntimeLighting() {
  ApplySceneBuildUpRuntimeVisualFromProfile();
}

void SceneBuildUp::DrawRuntimeEntities() {
  for (std::unique_ptr<Entity> &entity : entities_) {
    if (!entity) {
      continue;
    }
    if (entity->GetParent() == nullptr) {
      entity->Draw();
    }
  }
}

void SceneBuildUp::EnterScene() {
  EnsureCommonResources();
  TeardownEntities();

  sceneDocumentLoadFailed_ = false;
  sceneLoadFailureMessage_.clear();

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
  SceneBuildUpStageCamera::MaintainLockedStageCamera(*this);
  ApplySceneBuildUpRuntimeVisualFromProfile();
  SceneBuildUpAudioPolicy::OnEnterScene(*this);

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
  if (::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F1)) {
    Scene *const returnTarget =
        ResolveSceneBuildUpReturnTarget(returnScene_);
    Atrum::CommandChangeScene::Instance()->Set(
        returnTarget, std::make_unique<ChangeSceneFade>().release());
    return;
  }

  if (!sceneDocumentLoadFailed_) {
    for (std::unique_ptr<Entity> &entity : entities_) {
      if (entity && entity->IsActive()) {
        entity->Update();
      }
    }

    TickCameraRigEntities();

    Particle::TickEmitterManagerForActiveScene(GetCamera());
  }
  SceneBuildUpStageCamera::MaintainLockedStageCamera(*this);
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

  SetupRuntimeLighting();
  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);

  SceneBuildUpStageCamera::MaintainLockedStageCamera(*this);
  Atrum::Camera *camera = GetCamera();
  if (camera == nullptr) {
    return;
  }
  Editor::FitCameraPerspectiveToViewport(camera, width, height);
  engine->SetCamera(camera);
  DrawRuntimeEntities();
  Particle::DrawEmitterManagerForActiveScene();
  SceneBuildUpPlayerSilhouette::DrawAfterRuntimeEntities(engine, entities_);
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

  SetupRuntimeLighting();
  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);
  SceneBuildUpStageCamera::MaintainLockedStageCamera(*this);
  engine->SetCamera(GetCamera());
  DrawRuntimeEntities();
  Particle::DrawEmitterManagerForActiveScene();
  SceneBuildUpPlayerSilhouette::DrawAfterRuntimeEntities(engine, entities_);

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
  Atrum::CommandChangeScene::Instance()->Draw();
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (sceneDocumentLoadFailed_ && !sceneLoadFailureMessage_.empty()) {
    DrawSceneBuildUpSceneLoadFailureNotice(sceneLoadFailureMessage_);
  }
#endif
#ifdef USE_IMGUI
  engine->ImGuiRender();
#endif
  engine->PostDraw();
}

void SceneBuildUp::ExitScene() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  Editor::Scene::ClearStagedEditorPlayScenePath();
#endif
  SceneBuildUpAudioPolicy::OnExitScene(*this);
  TeardownEntities();
  ReleaseCommonResources();
  SceneBuildUpRegistry::Instance().OnSceneExited(this);
}

} // namespace Atrum
