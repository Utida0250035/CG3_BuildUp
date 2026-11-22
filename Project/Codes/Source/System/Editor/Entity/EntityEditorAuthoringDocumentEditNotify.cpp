#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor::EntityEditorAuthoringDocumentEditNotify {

void NotifyPreviewOnly(EntityEditor &editor) {
  UiState &ui = editor.Ui();
  ui.requestApplyPreview = true;
  editor.MarkSceneDocumentDirty();
}

void Notify(EntityEditor &editor) {
  NotifyPreviewOnly(editor);
  UiState &ui = editor.Ui();
  if (!ui.jsonFilePath.empty()) {
    ui.requestSaveToDisk = true;
  }
}

} // namespace Atrum::EntityEditor::EntityEditorAuthoringDocumentEditNotify