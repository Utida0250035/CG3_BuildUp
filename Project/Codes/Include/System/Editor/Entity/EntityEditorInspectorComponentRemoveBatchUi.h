#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
struct EntityDefinition;
struct ComponentSlot;
}

namespace Atrum::EntityEditor::EntityEditorInspectorComponentRemoveBatchUi {

/// 単体削除・同型一括・種別一括のコンポーネント削除ボタン
void Draw(EntityEditor &editor, EntityDefinition &document,
          const ComponentSlot &selectedSlot, bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorInspectorComponentRemoveBatchUi