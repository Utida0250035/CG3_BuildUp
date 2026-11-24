#include "System/Editor/Prefab/PrefabAssetProjectPanelList.h"

#include "System/Editor/Prefab/PrefabAssetFileOperations.h"
#include "System/Editor/Prefab/PrefabAssetUiPathScrub.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorProjectPanelSelectableLabel.h"

#include <cstring>
#include <filesystem>

#include <imgui/imgui.h>

namespace Atrum::Editor::Prefab::PrefabAssetProjectPanelList {

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

void DrawPrefabJsonFileList(EntityEditor::EntityEditor &editor,
                            const std::vector<std::string> &prefabPaths) {
  EntityEditor::UiState &ui = editor.Ui();

  static char renameBuffer[256]{};
  if (!ui.selectedPrefabPath.empty()) {
    static std::string lastSyncedPath{};
    if (lastSyncedPath != ui.selectedPrefabPath) {
      SyncRenameBuffer(ui.selectedPrefabPath, renameBuffer, sizeof(renameBuffer));
      lastSyncedPath = ui.selectedPrefabPath;
    }
  }

  ImGui::TextDisabled("クリックで選択（%zu 件）", prefabPaths.size());
  if (ImGui::BeginChild("PrefabJsonFileList", ImVec2(0.0f, 140.0f), true)) {
    for (const std::string &path : prefabPaths) {
      const bool isActive = ui.selectedPrefabPath == path;
      const std::string label =
          Atrum::EntityEditor::ProjectPanelSelectableLabel::
              BuildPrefabSelectableLabel(path);
      if (ImGui::Selectable(label.c_str(), isActive)) {
        ui.selectedPrefabPath = path;
        SyncRenameBuffer(ui.selectedPrefabPath, renameBuffer, sizeof(renameBuffer));
      }
      Atrum::EntityEditor::ProjectPanelSelectableLabel::DrawFullPathHoverTooltip(
          path);
    }
  }
  ImGui::EndChild();

  const bool hasSelection = !ui.selectedPrefabPath.empty();
  ImGui::BeginDisabled(!hasSelection);
  ImGui::InputText("新しいファイル名", renameBuffer, sizeof(renameBuffer));
  if (ImGui::Button("名前変更")) {
    const std::string oldPath = ui.selectedPrefabPath;
    const std::string newPath =
        TryRenamePrefabJsonFile(oldPath, std::string{renameBuffer});
    if (newPath.empty()) {
      ui.importStatusMessage = "プレハブ改名失敗: " + oldPath;
    } else {
      PrefabAssetUiPathScrub::OnPrefabPathRenamed(ui, oldPath, newPath);
      ui.importStatusMessage = "プレハブ改名: " + newPath;
      SyncRenameBuffer(ui.selectedPrefabPath, renameBuffer, sizeof(renameBuffer));
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("削除")) {
    const std::string path = ui.selectedPrefabPath;
    if (TryDeletePrefabJsonFile(path)) {
      PrefabAssetUiPathScrub::OnPrefabPathRemoved(ui, path);
      renameBuffer[0] = '\0';
      ui.importStatusMessage = "プレハブ削除: " + path;
    } else {
      ui.importStatusMessage = "プレハブ削除失敗: " + path;
    }
  }
  ImGui::EndDisabled();
}

} // namespace Atrum::Editor::Prefab::PrefabAssetProjectPanelList