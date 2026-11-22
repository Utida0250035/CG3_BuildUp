#include "System/Editor/Entity/EntityEditorTickPendingRequests.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor::EntityEditorTickPendingRequests {

bool Process(EntityEditor &editor) {
  bool needApply = false;
  UiState &ui = editor.Ui();

  if (ui.requestReloadFromDisk) {
    ui.requestReloadFromDisk = false;
    editor.LoadFromJsonPath(ui.jsonFilePath);
    needApply = true;
  }
  if (ui.requestSaveToDisk) {
    ui.requestSaveToDisk = false;
    if (!ui.jsonFilePath.empty()) {
      editor.SaveToJsonPath(ui.jsonFilePath);
      needApply = true;
    }
  }
  if (ui.requestApplyPreview) {
    ui.requestApplyPreview = false;
    needApply = true;
  }
  if (ui.requestResetPreview) {
    ui.requestResetPreview = false;
    needApply = true;
  }

  return needApply;
}

} // namespace Atrum::EntityEditor::EntityEditorTickPendingRequests