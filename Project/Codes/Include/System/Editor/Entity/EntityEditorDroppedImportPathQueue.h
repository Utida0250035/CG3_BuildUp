#pragma once

#include <string>
#include <vector>

namespace Atrum::EntityEditor {
struct UiState;
}

namespace Atrum::EntityEditor::EntityEditorDroppedImportPathQueue {

/// 1 回のドロップ処理でコピー成功した Imported パスをキューへ記録する。
void RecordBatch(UiState &ui, const std::vector<std::string> &importedPaths);

} // namespace Atrum::EntityEditor::EntityEditorDroppedImportPathQueue