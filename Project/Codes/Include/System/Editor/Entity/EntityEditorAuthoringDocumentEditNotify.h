#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorAuthoringDocumentEditNotify {

/// Authoring 編集後にプレビュー再構築・dirty フラグ・既知パスへの自動保存をキューする。
void Notify(EntityEditor &editor);

/// ドラッグ中など、毎フレームのプレビュー更新のみ（保存は Notify で確定時）。
void NotifyPreviewOnly(EntityEditor &editor);

} // namespace Atrum::EntityEditor::EntityEditorAuthoringDocumentEditNotify