#include "System/Editor/Entity/EntityEditorInspectorExecutionOrderBatchUi.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorExecutionOrderBatch.h"

#include <imgui/imgui.h>

#include <string>

namespace Atrum::EntityEditor::EntityEditorInspectorExecutionOrderBatchUi {

void Draw(EntityEditor &editor, EntityDefinition &document,
          ComponentSlot &selectedSlot, bool &outChanged) {
  UiState &ui = editor.Ui();

  int execOrder = selectedSlot.executionOrder;
  if (ImGui::InputInt("実行順 (priority)", &execOrder)) {
    selectedSlot.executionOrder = std::max(0, execOrder);
    outChanged = true;
  }

  if (ImGui::Button("この値を同型すべてに適用")) {
    const int applied =
        EntityEditorExecutionOrderBatch::ApplyExecutionOrderToAllSlotsOfType(
            document, selectedSlot.typeName, selectedSlot.executionOrder);
    if (applied > 1) {
      ui.importStatusMessage =
          "実行順を " + selectedSlot.typeName + " ×" +
          std::to_string(applied) + " に適用";
      outChanged = true;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("この値を選択種別すべてに適用")) {
    const int applied =
        EntityEditorExecutionOrderBatch::ApplyExecutionOrderToAllSlotsOfKind(
            document, ui.selectedComponentKind, selectedSlot.executionOrder);
    if (applied > 1) {
      ui.importStatusMessage =
          "実行順を種別コンポーネント " + std::to_string(applied) + " 件に適用";
      outChanged = true;
    }
  }

  if (ImGui::Button("選択種別を 0 から連番で整列")) {
    const int updated =
        EntityEditorExecutionOrderBatch::RenumberExecutionOrderSequentiallyForKind(
            document, ui.selectedComponentKind, 0);
    if (updated > 1) {
      ui.importStatusMessage =
          "実行順を連番整列: " + std::to_string(updated) + " 件";
      outChanged = true;
    }
  }
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorExecutionOrderBatchUi