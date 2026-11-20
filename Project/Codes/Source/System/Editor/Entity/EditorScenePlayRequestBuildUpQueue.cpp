#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Entity/EditorScenePlayRequestBuildUpQueue.h"

#include "Scene/CommandSceneDevelopmentBoot.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum::EntityEditor {

bool TrySetStartupSceneAndQueueBuildUp(EntityEditorUi &ui,
                                        const std::string &sceneJsonPath,
                                        std::string *statusMessage) {
  if (sceneJsonPath.empty()) {
    if (statusMessage != nullptr) {
      *statusMessage = "起動シーンのパスが空です";
    }
    return false;
  }
  if (!Atrum::Editor::Scene::TrySetRepositoryStartupScene(sceneJsonPath)) {
    if (statusMessage != nullptr) {
      *statusMessage =
          "起動シーンの保存に失敗しました（パスと JSON を確認してください）";
    }
    return false;
  }
  if (!IsValidBootSceneJsonPath(sceneJsonPath)) {
    if (statusMessage != nullptr) {
      *statusMessage =
          "起動 JSON を読み込めません: " + sceneJsonPath;
    }
    return false;
  }

  ui.sceneBuildUpLaunchPath = sceneJsonPath;
  ui.requestLaunchSceneBuildUp = true;
  if (statusMessage != nullptr) {
    *statusMessage =
        "起動シーンを保存し BuildUp を予約しました（Set → BuildUp）";
  }
  return true;
}

} // namespace Atrum::EntityEditor

#endif