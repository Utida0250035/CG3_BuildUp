#include "Scene/CommandScene.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneEntityEditor.h"
#else
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneTitle.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#endif

namespace Atrum {

/// <summary>
/// 指令類 > シーン指令 > コンストラクタ
/// </summary>
CommandScene::CommandScene() {

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  sceneNow_ = SceneEntityEditor::Instance();
#else
  const std::string startupScene = Editor::Scene::ResolveStartupScenePath();
  if (!startupScene.empty()) {
    sceneNow_ = SceneBuildUpRegistry::Instance().Launch(
        startupScene, SceneTitle::Instance());
  } else {
    sceneNow_ = SceneTitle::Instance();
  }
#endif

  sceneNow_->EnterScene();
}

} // namespace Atrum