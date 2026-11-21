#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorAddEmptyInstanceFromSelection {

/// 選択中の定義を複製した新インスタンスを追加する。シーンが空のときは既定定義。
void AppendInstance(EntityEditor &editor);

} // namespace Atrum::EntityEditor::EntityEditorAddEmptyInstanceFromSelection