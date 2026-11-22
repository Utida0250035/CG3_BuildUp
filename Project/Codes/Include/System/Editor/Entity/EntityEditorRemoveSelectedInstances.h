#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorRemoveSelectedInstances {

/// `selectedInstanceIndices` の全件を削除（最低 1 体は残す。全選択時は主選択を残す）。
void RemoveFromScene(EntityEditor &editor);

} // namespace Atrum::EntityEditor::EntityEditorRemoveSelectedInstances