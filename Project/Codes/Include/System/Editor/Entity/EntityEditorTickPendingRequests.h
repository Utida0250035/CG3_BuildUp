#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorTickPendingRequests {

/// UI 保留リクエスト（保存・再読込・プレビュー）を処理し、プレビュー再構築が要るとき true。
bool Process(EntityEditor &editor);

} // namespace Atrum::EntityEditor::EntityEditorTickPendingRequests