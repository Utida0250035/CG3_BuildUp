#include "System/Editor/Entity/EntityEditorDroppedImportPathQueue.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor::EntityEditorDroppedImportPathQueue {

void RecordBatch(UiState &ui, const std::vector<std::string> &importedPaths) {
  if (importedPaths.empty()) {
    return;
  }
  ui.recentImportedAssetPaths = importedPaths;
  ui.lastImportedAssetPath = importedPaths.back();
}

} // namespace Atrum::EntityEditor::EntityEditorDroppedImportPathQueue