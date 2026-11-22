#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <string>

namespace Atrum::EntityEditor::EntityEditorInspectorComponentTypeComboApply {

/// 型コンボ変更時に typeName と params を既定値へ丸ごと差し替える（旧型のキーは残さない）
void ApplyKnownComponentTypeFromCombo(ComponentSlot &slot, ComponentSlotKind kind,
                                    const std::string &newTypeName);

} // namespace Atrum::EntityEditor::EntityEditorInspectorComponentTypeComboApply