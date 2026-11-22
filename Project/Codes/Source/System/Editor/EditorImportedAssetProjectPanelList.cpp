#include "System/Editor/EditorImportedAssetProjectPanelList.h"

#include "System/Editor/EditorImportedAsset.h"
#include "System/Editor/EditorImportedAssetBatchApplyToSelection.h"
#include "System/Editor/EditorImportedAssetUiPathScrub.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <cstring>
#include <filesystem>

#include <imgui/imgui.h>

namespace Atrum::Editor::EditorImportedAssetProjectPanelList {

namespace {

void SyncRenameBuffer(const std::string &selectedPath, char *renameBuffer,
                      const size_t bufferSize) {
  if (selectedPath.empty()) {
    renameBuffer[0] = '\0';
    return;
  }
  const std::string fileName =
      std::filesystem::path(selectedPath).filename().string();
  const size_t copyCount =
      fileName.size() < bufferSize - 1 ? fileName.size() : bufferSize - 1;
  std::memcpy(renameBuffer, fileName.data(), copyCount);
  renameBuffer[copyCount] = '\0';
}

} // namespace

void DrawImportedAssetsFolderList(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();
  const std::vector<std::string> importedFiles =
      Import::ListImportedAssetRelativePaths();

  ImGui::Separator();
  ImGui::TextUnformatted("Imported フォルダ一覧");
  ImGui::TextDisabled("クリックで直近パスに設定（%zu 件）", importedFiles.size());

  static std::string selectedListPath{};
  static char renameBuffer[256]{};
  if (ImGui::BeginChild("ImportedFolderList", ImVec2(0.0f, 120.0f), true)) {
    for (const std::string &path : importedFiles) {
      const bool selected = selectedListPath == path;
      if (ImGui::Selectable(path.c_str(), selected)) {
        selectedListPath = path;
        ui.lastImportedAssetPath = path;
        SyncRenameBuffer(selectedListPath, renameBuffer, sizeof(renameBuffer));
      }
    }
  }
  ImGui::EndChild();

  const bool hasSelection = !selectedListPath.empty();
  ImGui::BeginDisabled(!hasSelection);
  if (ImGui::Button("一覧の選択をインスタンスへ適用")) {
    const int count = EditorImportedAssetBatchApplyToSelection::
        ApplySinglePathToSelectedInstances(editor, selectedListPath);
    ui.importStatusMessage =
        "一覧から適用 " + std::to_string(count) + " 体: " + selectedListPath;
  }
  ImGui::EndDisabled();

  ImGui::BeginDisabled(!hasSelection);
  ImGui::InputText("新しいファイル名", renameBuffer, sizeof(renameBuffer));
  if (ImGui::Button("名前変更")) {
    const std::string oldPath = selectedListPath;
    const std::string newPath = Import::TryRenameImportedAssetFile(
        oldPath, std::string{renameBuffer});
    if (newPath.empty()) {
      ui.importStatusMessage = "Imported 改名失敗: " + oldPath;
    } else {
      EditorImportedAssetUiPathScrub::OnImportedAssetPathRenamed(ui, oldPath,
                                                                 newPath);
      selectedListPath = newPath;
      ui.importStatusMessage = "Imported 改名: " + newPath;
      SyncRenameBuffer(selectedListPath, renameBuffer, sizeof(renameBuffer));
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("削除")) {
    const std::string path = selectedListPath;
    if (Import::TryDeleteImportedAssetFile(path)) {
      EditorImportedAssetUiPathScrub::OnImportedAssetPathRemoved(ui, path);
      selectedListPath.clear();
      renameBuffer[0] = '\0';
      ui.importStatusMessage = "Imported 削除: " + path;
    } else {
      ui.importStatusMessage = "Imported 削除失敗: " + path;
    }
  }
  ImGui::EndDisabled();
}

} // namespace Atrum::Editor::EditorImportedAssetProjectPanelList