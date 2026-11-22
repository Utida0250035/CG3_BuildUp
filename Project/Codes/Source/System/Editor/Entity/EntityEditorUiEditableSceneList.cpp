#include "System/Editor/Entity/EntityEditorUiEditableSceneList.h"

#include "System/Editor/EditorAssetsPaths.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/GameAssetsPaths.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "ForDebug/ImGui.h"
#include <string>
#include <vector>

namespace Atrum::EntityEditor {

namespace {

bool PathIsUnderDebugPlayArea(const std::string &path) {
  return path.find("DebugPlayArea") != std::string::npos ||
         path.find(EditorAssets::kDebugPlayAreaDirectory) != std::string::npos;
}

void DrawScenePathRows(UiState &ui, const std::vector<std::string> &paths) {
  for (const std::string &path : paths) {
    const bool isActive = !ui.selectedScenePath.empty()
                              ? ui.selectedScenePath == path
                              : ui.jsonFilePath == path;
    const std::string label =
        Atrum::Editor::Scene::SceneDisplayNameFromPath(path) + "  |  " + path;
    if (ImGui::Selectable(label.c_str(), isActive)) {
      ui.selectedScenePath = path;
    }
  }
}

} // namespace

void DrawEntityEditorEditableSceneList(EntityEditor &editor) {
  UiState &ui = editor.Ui();

  const std::vector<std::string> scenePaths =
      Atrum::Editor::Scene::ListEditableSceneJsonFiles();
  if (scenePaths.empty()) {
    ImGui::TextDisabled(
        "シーンがありません: %s / %s / %s", Atrum::GameAssets::kScenesDirectory,
        Atrum::Editor::Scene::kScenesDirectory, EditorAssets::kDebugPlayAreaDirectory);
    return;
  }

  std::vector<std::string> playAreaPaths;
  std::vector<std::string> otherPaths;
  playAreaPaths.reserve(scenePaths.size());
  otherPaths.reserve(scenePaths.size());
  for (const std::string &path : scenePaths) {
    if (PathIsUnderDebugPlayArea(path)) {
      playAreaPaths.push_back(path);
    } else {
      otherPaths.push_back(path);
    }
  }

  if (!otherPaths.empty()) {
    if (ImGui::TreeNodeEx("ゲーム／エディタシーン",
                          ImGuiTreeNodeFlags_DefaultOpen)) {
      DrawScenePathRows(ui, otherPaths);
      ImGui::TreePop();
    }
  }

  if (!playAreaPaths.empty()) {
    if (ImGui::TreeNodeEx("テスト再生エリア (DebugPlayArea)",
                          ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::TextDisabled("scaffold 出力・FeatureTest 等はここに表示されます");
      DrawScenePathRows(ui, playAreaPaths);
      ImGui::TreePop();
    }
  }
}

} // namespace Atrum::EntityEditor

#else

namespace Atrum::EntityEditor {

void DrawEntityEditorEditableSceneList(EntityEditor &) {}

} // namespace Atrum::EntityEditor

#endif