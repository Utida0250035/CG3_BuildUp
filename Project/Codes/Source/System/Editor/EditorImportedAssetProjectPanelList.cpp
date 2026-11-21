#include "System/Editor/EditorImportedAssetProjectPanelList.h"

#include "System/Editor/EditorImportedAsset.h"
#include "System/Editor/EditorImportedAssetBatchApplyToSelection.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <imgui.h>

namespace Atrum::Editor::EditorImportedAssetProjectPanelList {

void DrawImportedAssetsFolderList(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();
  const std::vector<std::string> importedFiles =
      Import::ListImportedAssetRelativePaths();

  ImGui::Separator();
  ImGui::TextUnformatted("Imported フォルダ一覧");
  ImGui::TextDisabled("クリックで直近パスに設定（%zu 件）", importedFiles.size());

  static std::string selectedListPath{};
  if (ImGui::BeginChild("ImportedFolderList", ImVec2(0.0f, 120.0f), true)) {
    for (const std::string &path : importedFiles) {
      const bool selected = selectedListPath == path;
      if (ImGui::Selectable(path.c_str(), selected)) {
        selectedListPath = path;
        ui.lastImportedAssetPath = path;
      }
    }
  }
  ImGui::EndChild();

  ImGui::BeginDisabled(selectedListPath.empty());
  if (ImGui::Button("一覧の選択をインスタンスへ適用")) {
    const int count = EditorImportedAssetBatchApplyToSelection::
        ApplySinglePathToSelectedInstances(editor, selectedListPath);
    ui.importStatusMessage =
        "一覧から適用 " + std::to_string(count) + " 体: " + selectedListPath;
  }
  ImGui::EndDisabled();
}

} // namespace Atrum::Editor::EditorImportedAssetProjectPanelList