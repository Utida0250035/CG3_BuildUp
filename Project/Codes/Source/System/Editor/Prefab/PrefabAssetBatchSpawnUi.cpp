#include "System/Editor/Prefab/PrefabAssetBatchSpawnUi.h"

#include "System/Editor/Entity/EntityEditorPrefabBatchSpawnFromFile.h"
#include "System/Editor/Entity/EntityEditor.h"

#include <algorithm>
#include <imgui/imgui.h>

namespace Atrum::Editor::Prefab::PrefabAssetBatchSpawnUi {

void DrawBatchSpawnControls(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();
  if (ui.selectedPrefabPath.empty()) {
    return;
  }

  ImGui::InputInt("追加 件数", &ui.prefabSpawnBatchCount, 1, 5);
  ui.prefabSpawnBatchCount =
      std::clamp(ui.prefabSpawnBatchCount, 1, 256);
  ImGui::InputFloat("X 間隔", &ui.prefabSpawnSpacingX, 0.05f, 0.25f, "%.2f");

  if (ImGui::Button("一括配置（シーンへ）")) {
    const int spawned =
        EntityEditor::EntityEditorPrefabBatchSpawnFromFile::
            AppendFromPrefabFileBatch(editor, ui.selectedPrefabPath,
                                      ui.prefabSpawnBatchCount,
                                      ui.prefabSpawnSpacingX);
    ui.importStatusMessage =
        "プレハブ一括配置: " + std::to_string(spawned) + " 体 (" +
        ui.selectedPrefabPath + ")";
  }
}

} // namespace Atrum::Editor::Prefab::PrefabAssetBatchSpawnUi