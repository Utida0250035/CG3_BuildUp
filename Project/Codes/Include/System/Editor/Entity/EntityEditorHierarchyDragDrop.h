#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorHierarchyDragDrop {

/// ヒエラルキー Selectable 直後に呼ぶ（ドラッグ複製・並べ替え・親変更）
void AttachToHierarchyNode(EntityEditor &editor, int nodeIndex);

/// ルート TreeNode 上へのドロップ（親をなしに戻す）
void AttachRootDropTarget(EntityEditor &editor);

} // namespace Atrum::EntityEditor::EntityEditorHierarchyDragDrop