#pragma once

namespace Atrum::EntityEditor {

class EntityEditor;
struct EntityDefinition;
struct ComponentSlot;

namespace EntityEditorInspectorMultiSlotExecutionOrderUi {

/// 複数スロットの同時実行順編集および複数インスタンス同型スロット一括適用 UI
void Draw(EntityEditor &editor, EntityDefinition &document,
          const ComponentSlot &selectedSlot, bool &outChanged);

} // namespace EntityEditorInspectorMultiSlotExecutionOrderUi

} // namespace Atrum::EntityEditor
