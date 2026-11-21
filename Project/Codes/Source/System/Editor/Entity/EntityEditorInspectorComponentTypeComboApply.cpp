#include "System/Editor/Entity/EntityEditorInspectorComponentTypeComboApply.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum::EntityEditor::EntityEditorInspectorComponentTypeComboApply {

void ApplyKnownComponentTypeFromCombo(ComponentSlot &slot,
                                      const ComponentSlotKind kind,
                                      const std::string &newTypeName) {
  slot.kind = kind;
  slot.typeName = newTypeName;
  slot.params = DefaultParamsForType(newTypeName);
}

} // namespace Atrum::EntityEditor::EntityEditorInspectorComponentTypeComboApply