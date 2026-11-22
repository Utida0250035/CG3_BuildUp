#include "System/Editor/Entity/EntityEditorDroppedImportRequestPreview.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

namespace Atrum::EntityEditor::EntityEditorDroppedImportRequestPreview {

void AfterProcessing(EntityEditor &editor, const int spawnedInstanceCount,
                     const std::vector<std::string> &importedPaths) {
  if (importedPaths.empty()) {
    return;
  }

  UiState &ui = editor.Ui();
  ui.requestApplyPreview = true;

  if (spawnedInstanceCount > 0) {
    return;
  }

  EntityEditorAuthoringDocumentEditNotify::NotifyPreviewOnly(editor);
}

} // namespace Atrum::EntityEditor::EntityEditorDroppedImportRequestPreview