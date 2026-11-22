#include "System/Editor/Entity/EntityEditorInspectorComponentRemoveBatchUi.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
#include "System/Editor/Entity/EntityEditorRemoveComponentSlotsBatch.h"

#include <imgui.h>

namespace Atrum::EntityEditor::EntityEditorInspectorComponentRemoveBatchUi {

void Draw(EntityEditor &editor, EntityDefinition &document,
          const ComponentSlot &selectedSlot, bool &outChanged) {
  UiState &ui = editor.Ui();

  if (ImGui::Button("選択中のコンポーネントを削除")) {
    if (EntityEditorRemoveComponentSlotsBatch::RemoveComponentSlotAtIndex(
            document, ui.selectedComponentIndex)) {
      EntityEditorAuthoringDocumentEditNotify::Notify(editor);
      outChanged = true;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("同じ型をすべて削除")) {
    const int removed =
        EntityEditorRemoveComponentSlotsBatch::RemoveAllComponentSlotsOfType(
            document, selectedSlot.typeName);
    if (removed > 0) {
      ui.importStatusMessage =
          "コンポーネント削除: " + selectedSlot.typeName + " ×" +
          std::to_string(removed);
      EntityEditorAuthoringDocumentEditNotify::Notify(editor);
      outChanged = true;
    }
  }

  if (ImGui::Button("選択種別のコンポーネントをすべて削除")) {
    const int removed =
        EntityEditorRemoveComponentSlotsBatch::RemoveAllComponentSlotsOfKind(
            document, ui.selectedComponentKind);
    if (removed > 0) {
      ui.importStatusMessage =
          "種別コンポーネント削除: " + std::to_string(removed) + " 件";
      EntityEditorAuthoringDocumentEditNotify::Notify(editor);
      outChanged = true;
    }
  }
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorComponentRemoveBatchUi