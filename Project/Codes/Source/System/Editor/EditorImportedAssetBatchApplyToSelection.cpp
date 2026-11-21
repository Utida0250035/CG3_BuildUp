#include "System/Editor/EditorImportedAssetBatchApplyToSelection.h"

#include "System/Editor/EditorImportedAssetApplyToEntityDefinition.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

#include <algorithm>

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "ForDebug/ImGui.h"
#endif

namespace Atrum::Editor::EditorImportedAssetBatchApplyToSelection {

namespace {

std::vector<int> CollectTargetInstanceIndices(
    const EntityEditor::EntityEditor &editor) {
  const EntityEditor::UiState &ui = editor.Ui();
  const EntityEditor::EditorSceneDocument &scene = editor.SceneDocument();
  std::vector<int> indices = ui.selectedInstanceIndices;
  if (indices.empty() && ui.selectedInstanceIndex >= 0) {
    indices.push_back(ui.selectedInstanceIndex);
  }
  std::sort(indices.begin(), indices.end());
  indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
  indices.erase(std::remove_if(indices.begin(), indices.end(),
                               [&scene](const int index) {
                                 return index < 0 ||
                                        index >= static_cast<int>(
                                                    scene.instances.size());
                               }),
              indices.end());
  return indices;
}

int ApplyPathsToIndices(EntityEditor::EntityEditor &editor,
                        const std::vector<int> &indices,
                        const std::vector<std::string> &paths) {
  if (indices.empty() || paths.empty()) {
    return 0;
  }

  EntityEditor::EditorSceneDocument &scene = editor.SceneDocument();
  int applied = 0;
  for (size_t i = 0; i < indices.size(); ++i) {
    const std::string &path = paths[i % paths.size()];
    if (!EditorImportedAssetApplyToEntityDefinition::IsSupportedAssetExtension(
            path)) {
      continue;
    }
    EntityEditor::SceneEntityInstance &instance =
        scene.instances[static_cast<size_t>(indices[i])];
    if (EditorImportedAssetApplyToEntityDefinition::ApplyImportedPath(
            instance.definition, path)) {
      ++applied;
    }
  }

  if (applied > 0) {
    EntityEditor::EntityEditorAuthoringDocumentEditNotify::Notify(editor);
  }
  return applied;
}

} // namespace

int ApplySinglePathToSelectedInstances(EntityEditor::EntityEditor &editor,
                                       const std::string &importedPath) {
  if (importedPath.empty()) {
    return 0;
  }
  return ApplyPathsToIndices(editor, CollectTargetInstanceIndices(editor),
                             {importedPath});
}

int ApplyPathQueueToSelectedInstances(
    EntityEditor::EntityEditor &editor,
    const std::vector<std::string> &importedPaths) {
  if (importedPaths.empty()) {
    return 0;
  }
  return ApplyPathsToIndices(editor, CollectTargetInstanceIndices(editor),
                             importedPaths);
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

void DrawProjectPanelBatchApplyUi(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();
  const std::vector<int> targets = CollectTargetInstanceIndices(editor);
  const bool hasSelection = !targets.empty();
  const bool hasLast = !ui.lastImportedAssetPath.empty();
  const bool hasQueue = !ui.recentImportedAssetPaths.empty();

  if (!hasLast && !hasQueue) {
    return;
  }

  ImGui::Separator();
  ImGui::TextUnformatted("Imported 一括適用（選択中へ）");
  if (!hasSelection) {
    ImGui::TextDisabled("ヒエラルキーでインスタンスを選択してください");
  }

  ImGui::BeginDisabled(!hasSelection || !hasLast);
  if (ImGui::Button("直近パスを選択へ一括適用")) {
    const int count = ApplySinglePathToSelectedInstances(
        editor, ui.lastImportedAssetPath);
    ui.importStatusMessage =
        "選択 " + std::to_string(count) + " 体へ適用: " + ui.lastImportedAssetPath;
  }
  ImGui::EndDisabled();

  ImGui::BeginDisabled(!hasSelection || !hasQueue);
  if (ImGui::Button("キューを選択へ順に一括適用")) {
    const int count =
        ApplyPathQueueToSelectedInstances(editor, ui.recentImportedAssetPaths);
    ui.importStatusMessage =
        "選択 " + std::to_string(count) + " 体へキュー " +
        std::to_string(ui.recentImportedAssetPaths.size()) + " 件を割当";
  }
  ImGui::EndDisabled();
}

#endif

} // namespace Atrum::Editor::EditorImportedAssetBatchApplyToSelection