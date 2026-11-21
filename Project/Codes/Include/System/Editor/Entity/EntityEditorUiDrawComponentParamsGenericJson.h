#pragma once

namespace Atrum::EntityEditor {
struct ComponentSlot;
}

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsGenericJson {

/// 登録外 typeName 向けの params JSON 簡易エディタ（component_split_guidelines 参照を案内）
bool TryDrawUnknownComponentParams(ComponentSlot &slot, bool &outChanged);

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsGenericJson