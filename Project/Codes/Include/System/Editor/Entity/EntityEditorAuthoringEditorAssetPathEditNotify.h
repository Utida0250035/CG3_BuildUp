#pragma once

namespace Atrum::EntityEditor {

class EntityEditor;

} // namespace Atrum::EntityEditor

namespace Atrum::EntityEditor::EntityEditorAuthoringEditorAssetPathEditNotify {

/// CmpEditorAssetPath のインスペクタ編集はプレビュー再構築のみ（保存は別操作）。
void NotifyEditorAssetPathPreviewRefresh(EntityEditor &editor);

} // namespace Atrum::EntityEditor::EntityEditorAuthoringEditorAssetPathEditNotify