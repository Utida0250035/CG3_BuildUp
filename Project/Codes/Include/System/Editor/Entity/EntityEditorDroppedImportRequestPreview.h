#pragma once

#include <cstddef>
#include <vector>
#include <string>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorDroppedImportRequestPreview {

/// ドロップ処理後にプレビュー再構築をキューする（spawn 無しのコピー専用時も）。
void AfterProcessing(EntityEditor &editor, int spawnedInstanceCount,
                     const std::vector<std::string> &importedPaths);

} // namespace Atrum::EntityEditor::EntityEditorDroppedImportRequestPreview