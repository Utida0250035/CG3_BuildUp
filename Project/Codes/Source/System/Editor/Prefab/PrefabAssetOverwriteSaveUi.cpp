#include "System/Editor/Prefab/PrefabAssetOverwriteSaveUi.h"

#include "System/Editor/Prefab/PrefabAssetFileOperations.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <imgui.h>

namespace Atrum::Editor::Prefab::PrefabAssetOverwriteSaveUi {

void DrawOverwriteSelectedPrefabSaveButton(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();
  const EditorSceneDocument &scene = editor.SceneDocument();
  if (ui.selectedPrefabPath.empty() || scene.instances.empty()) {
    return;
  }

  ImGui::BeginDisabled(
      !IsManagedExistingPrefabJsonPath(ui.selectedPrefabPath));
  if (ImGui::Button("選択プレハブへ上書き保存")) {
    if (IsManagedExistingPrefabJsonPath(ui.selectedPrefabPath) &&
        editor.SaveSelectedInstanceToPrefabPath(ui.selectedPrefabPath)) {
      ui.importStatusMessage =
          "プレハブ上書き: " + ui.selectedPrefabPath;
    } else {
      ui.importStatusMessage =
          "プレハブ上書き失敗: " + ui.selectedPrefabPath;
    }
  }
  ImGui::EndDisabled();
  if (!ui.selectedPrefabPath.empty() &&
      !IsManagedExistingPrefabJsonPath(ui.selectedPrefabPath)) {
    ImGui::TextDisabled("上書き先が無効、または一覧外のパスです");
  }
}

} // namespace Atrum::Editor::Prefab::PrefabAssetOverwriteSaveUi