#include "Scene/CommandScene.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneEntityEditor.h"
#include "Scene/SceneTitle.h"
#else
#include "Scene/CommandSceneDevelopmentBoot.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneTitle.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#endif

namespace Atrum {

/// <summary>
/// 指令類 > シーン指令 > コンストラクタ
/// </summary>
CommandScene::CommandScene() {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  SceneEntityEditor *const entityEditor = SceneEntityEditor::Instance();
  const DevelopmentBootTarget bootTarget = ResolveDevelopmentBootTarget();
  if (bootTarget.kind == DevelopmentBootKind::SceneBuildUp) {
    sceneNow_ = SceneBuildUpRegistry::Instance().Launch(
        bootTarget.sceneJsonPath, entityEditor);
  } else if (bootTarget.kind == DevelopmentBootKind::SceneBuildUpPlayRuntime) {
    sceneNow_ = SceneBuildUpRegistry::Instance().Launch(
        bootTarget.sceneJsonPath, SceneTitle::Instance());
  } else if (bootTarget.kind == DevelopmentBootKind::EntityEditorWithJson) {
    entityEditor->SetBootJsonPath(bootTarget.sceneJsonPath);
    sceneNow_ = entityEditor;
  } else {
    sceneNow_ = entityEditor;
  }
#else
  const std::string startupScene = Editor::Scene::ResolveStartupScenePath();
  if (!startupScene.empty() && IsValidBootSceneJsonPath(startupScene)) {
    sceneNow_ = SceneBuildUpRegistry::Instance().Launch(
        startupScene, SceneTitle::Instance());
  } else if (IsValidBootSceneJsonPath(EditorAssets::kDebugPlayAreaTemplateScene)) {
    sceneNow_ = SceneBuildUpRegistry::Instance().Launch(
        EditorAssets::kDebugPlayAreaTemplateScene, SceneTitle::Instance());
  } else {
    sceneNow_ = SceneTitle::Instance();
  }
#endif

  sceneNow_->EnterScene();
}

} // namespace Atrum