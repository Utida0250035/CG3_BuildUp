#include "System/Editor/Prefab/PrefabAssetBatchSpawnUi.h"

#include "System/Editor/Entity/EntityEditorPrefabBatchSpawnFromFile.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <imgui.h>

namespace Atrum::Editor::Prefab::PrefabAssetBatchSpawnUi {

void DrawBatchSpawnControls(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();
  if (ui.selectedPrefabPath.empty()) {
    return;
  }

  static int batchCount = 5;
  static float batchSpacing = 1.25f;
  ImGui::InputInt("一括配置 件数", &batchCount, 1, 5);
  if (batchCount < 1) {
    batchCount = 1;
  }
  if (batchCount > 256) {
    batchCount = 256;
  }
  ImGui::InputFloat("X 間隔", &batchSpacing, 0.05f, 0.25f, "%.2f");

  if (ImGui::Button("一括配置（シーンへ）")) {
    const int spawned =
        EntityEditor::EntityEditorPrefabBatchSpawnFromFile::
            AppendFromPrefabFileBatch(editor, ui.selectedPrefabPath, batchCount,
                                      batchSpacing);
    ui.importStatusMessage =
        "プレハブ一括配置: " + std::to_string(spawned) + " 体 (" +
        ui.selectedPrefabPath + ")";
  }
}

} // namespace Atrum::Editor::Prefab::PrefabAssetBatchSpawnUi