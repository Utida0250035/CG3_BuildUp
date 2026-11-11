#include "Scene/SceneBuildUp.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/LightData.h"
#include "Input/PlayInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Entity/Entity.h"

#include <algorithm>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#include "System/Editor/EditorFrame.h"
#include "System/Editor/EditorSceneRunStatus.h"
#include "System/Editor/EditorSceneViewPanel.h"
#include "Scene/CommandChangeScene.h"
#include "System/Editor/EditorSceneRenderTarget.h"
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
  auto scene = std::make_unique<SceneBuildUp>(sceneJsonPath, returnScene);
  Scene *raw = scene.get();
  ownedScenes_.push_back(std::move(scene));
  return raw;
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

  EntityEditor::EditorSceneDocument document{};
  if (!EntityEditor::TryLoadSceneDocument(sceneJsonPath_, document)) {
    document.sceneName = sceneDisplayName_;
    EntityEditor::SceneEntityInstance instance{};
    instance.instanceName = "Entity_0";
    instance.definition =
        EntityEditor::MakeDefaultEntityDefinition("Entity_0");
    document.instances = {std::move(instance)};
  }

  if (!document.sceneName.empty()) {
    sceneDisplayName_ = document.sceneName;
  }

  entities_ = EntityEditor::BuildEntitiesFromSceneDocument(document);
  SetupStageViewCamera();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  Editor::EditorFrame::Instance().SetSceneViewTab(Editor::SceneViewTab::Runtime);
#endif
}

void SceneBuildUp::Update() {
  if (returnScene_ != nullptr &&
      ::Atrum::EnginePlayInput()->IsKeyTrigger(Atrum::Input::Key::F1)) {
    Atrum::CommandChangeScene::Instance()->Set(
        returnScene_, std::make_unique<ChangeSceneFade>().release());
    return;
  }

  for (std::unique_ptr<Entity> &entity : entities_) {
    if (entity && entity->IsActive()) {
      entity->Update();
    }
  }
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

  SetupRuntimeLighting();
  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);

  SetupStageViewCamera();
  Atrum::Camera *camera = GetCamera();
  if (camera == nullptr) {
    return;
  }
  Editor::FitCameraPerspectiveToViewport(camera, width, height);
  engine->SetCamera(camera);
  DrawRuntimeEntities();
}

#endif

void SceneBuildUp::DrawScene() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  return;
#else
  Atrum::Engine *engine = ::Atrum::EngineInstance();
  if (engine == nullptr) {
    return;
  }

  SetupRuntimeLighting();
  engine->SetBlendModeForFrame(Atrum::BlendMode::NORMAL);
  engine->SetCamera(GetCamera());
  DrawRuntimeEntities();
#endif
}

void SceneBuildUp::Draw() {
  Atrum::Engine *engine = ::Atrum::EngineInstance();

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  Editor::EditorFrame::Instance().ResetPerFrame();
  const Editor::EditorSceneRunPhase runPhase =
      Atrum::CommandChangeScene::Instance()->GetIsExecute()
          ? Editor::EditorSceneRunPhase::Waiting
          : Editor::EditorSceneRunPhase::Running;
  Editor::LayoutDedicatedSceneViewWindow(
      Editor::kRuntimeSceneViewTitle, Editor::SceneViewTab::Runtime,
      "実行時ビュー — ランタイム（ステージカメラ・シミュレーション実行）",
      "F1: エンティティエディタへ戻る", true, runPhase);
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
  Editor::EditorSceneRenderTargetHub::Instance().RenderSceneViews(
      engine, [this](const Editor::SceneViewTab tab, const int width,
                     const int height) {
        if (tab == Editor::SceneViewTab::Runtime) {
          DrawRuntimeViewContent(width, height);
        }
      });
#endif

  engine->PostDraw();
}

void SceneBuildUp::ExitScene() {
  TeardownEntities();
  ReleaseCommonResources();
  SceneBuildUpRegistry::Instance().OnSceneExited(this);
}

} // namespace Atrum
