#pragma once

namespace Atrum::EntityEditor {
struct EditorSceneDocument;
}

namespace Atrum::EntityEditor::EntityEditorHierarchyInstanceReorder {

/// 同一親前提で afterIndex の直後へ並べ替え。成功時は from の新インデックス
bool MoveAfterSibling(EditorSceneDocument &scene, int fromIndex, int afterIndex,
                      int *outNewFromIndex = nullptr);

} // namespace Atrum::EntityEditor::EntityEditorHierarchyInstanceReorder