#pragma once

namespace Atrum::EntityEditor {
struct EditorSceneDocument;
}

namespace Atrum::EntityEditor::EntityEditorHierarchyReparent {

bool WouldCreateParentCycle(const EditorSceneDocument &scene, int childIndex,
                            int newParentIndex);

/// child を newParent の子にする（-1 はルート）
bool Reparent(EditorSceneDocument &scene, int childIndex, int newParentIndex);

} // namespace Atrum::EntityEditor::EntityEditorHierarchyReparent