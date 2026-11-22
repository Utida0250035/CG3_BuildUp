#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorInspectorExecutionOrderBatchUi {

/// 選択スロットの実行順 InputInt と同型／種別への一括適用・連番整列
void Draw(EntityEditor &editor, EntityDefinition &document,
          ComponentSlot &selectedSlot, bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorInspectorExecutionOrderBatchUi