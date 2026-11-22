#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorHierarchyDuplicateAsChild {

/// source を複製し parentIndex を newParentIndex に設定（-1 でルート）
bool DuplicateUnderParent(EntityEditor &editor, int sourceIndex,
                          int newParentIndex);

} // namespace Atrum::EntityEditor::EntityEditorHierarchyDuplicateAsChild