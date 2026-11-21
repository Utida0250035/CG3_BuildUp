#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/Entity/EntityEditorStartupSceneSetWithoutAutosave.h"

#include "Scene/CommandSceneDevelopmentBoot.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum::EntityEditor::EntityEditorStartupSceneSetWithoutAutosave {

std::string ResolveStartupTargetPath(const EntityEditor &editor) {
  const UiState &ui = editor.Ui();
  if (!ui.jsonFilePath.empty()) {
    return ui.jsonFilePath;
  }
  return ui.selectedScenePath;
}

bool SetOnly(EntityEditor &editor, std::string *statusMessage) {
  const std::string targetPath = ResolveStartupTargetPath(editor);
  if (targetPath.empty()) {
    if (statusMessage != nullptr) {
      *statusMessage = "起動シーンのパスが空です";
    }
    return false;
  }
  if (!Atrum::Editor::Scene::TrySetRepositoryStartupScene(targetPath)) {
    if (statusMessage != nullptr) {
      *statusMessage =
          "起動シーンの保存に失敗しました（パスと JSON を確認してください）";
    }
    return false;
  }
  if (statusMessage != nullptr) {
    *statusMessage =
        "Release 起動シーンを launch_config.json に保存しました（シーン "
        "JSON は自動保存しません）";
  }
  return true;
}

bool SetAndQueueBuildUp(EntityEditor &editor, std::string *statusMessage) {
  const std::string targetPath = ResolveStartupTargetPath(editor);
  if (targetPath.empty()) {
    if (statusMessage != nullptr) {
      *statusMessage = "起動シーンのパスが空です";
    }
    return false;
  }
  if (!Atrum::Editor::Scene::TrySetRepositoryStartupScene(targetPath)) {
    if (statusMessage != nullptr) {
      *statusMessage =
          "起動シーンの保存に失敗しました（パスと JSON を確認してください）";
    }
    return false;
  }
  if (!IsValidBootSceneJsonPath(targetPath)) {
    if (statusMessage != nullptr) {
      *statusMessage = "起動 JSON を読み込めません: " + targetPath;
    }
    return false;
  }

  UiState &ui = editor.Ui();
  if (!ui.jsonFilePath.empty() && ui.jsonFilePath == targetPath) {
    ui.requestSaveToDisk = true;
  }
  ui.sceneBuildUpLaunchPath = targetPath;
  ui.requestLaunchSceneBuildUp = true;
  if (statusMessage != nullptr) {
    *statusMessage =
        "起動シーンを設定し BuildUp を予約しました（編集中 JSON "
        "は保存キュー、launch のみ即時）";
  }
  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorStartupSceneSetWithoutAutosave

#endif