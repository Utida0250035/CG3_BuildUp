#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorCreateNewSceneFile {

/// 新規シーン JSON パスを割り当てる。ディスク保存はキューせずプレビューのみ更新。
void Create(EntityEditor &editor);

} // namespace Atrum::EntityEditor::EntityEditorCreateNewSceneFile