#include "System/Editor/Entity/EntityEditorInspectorImportedDropBatchApplyUi.h"

#include "System/Editor/EditorImportedAssetApplyToEntityDefinition.h"
#include "System/Editor/EditorImportedAssetBatchApplyToSelection.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

#include <imgui.h>

namespace Atrum::EntityEditor::EntityEditorInspectorImportedDropBatchApplyUi {

void Draw(EntityEditor &editor) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  UiState &ui = editor.Ui();
  if (ui.lastImportedAssetPath.empty()) {
    return;
  }

  ImGui::Separator();
  ImGui::TextUnformatted("直近ドロップ一括適用");

  ImGui::BeginDisabled(
      !EditorImportedAssetApplyToEntityDefinition::IsSupportedAssetExtension(
          ui.lastImportedAssetPath));
  if (ImGui::Button("この定義の全パススロットへ適用")) {
    const int slots =
        EditorImportedAssetApplyToEntityDefinition::
            ApplyImportedPathToAllMatchingSlots(editor.SelectedDefinition(),
                                            ui.lastImportedAssetPath);
    if (slots > 0) {
      EntityEditorAuthoringDocumentEditNotify::Notify(editor);
      ui.importStatusMessage =
          "パススロット " + std::to_string(slots) + " 件へ適用";
    } else if (EditorImportedAssetApplyToEntityDefinition::ApplyImportedPath(
                   editor.SelectedDefinition(), ui.lastImportedAssetPath)) {
      EntityEditorAuthoringDocumentEditNotify::Notify(editor);
      ui.importStatusMessage = "新規 Cmp を追加してパスを適用";
    } else {
      ui.importStatusMessage = "適用できませんでした";
    }
  }
  ImGui::EndDisabled();

  const size_t selectionCount = ui.selectedInstanceIndices.size();
  if (selectionCount > 1) {
    if (ImGui::Button("選択インスタンスへ一括適用（各定義の全スロット）")) {
      const int count = Editor::EditorImportedAssetBatchApplyToSelection::
          ApplySinglePathToSelectedInstances(editor, ui.lastImportedAssetPath);
      ui.importStatusMessage =
          "選択 " + std::to_string(count) + " 体へ適用";
    }
  }
#endif
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorImportedDropBatchApplyUi