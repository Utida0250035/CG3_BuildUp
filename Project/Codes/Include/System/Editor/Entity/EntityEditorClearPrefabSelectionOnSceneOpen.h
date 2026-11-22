#pragma once

namespace Atrum::EntityEditor {
struct UiState;
}

namespace Atrum::EntityEditor::EntityEditorClearPrefabSelectionOnSceneOpen {

/// シーンドキュメントを開いた直後にプレハブ一覧選択をリセットする
void OnSceneDocumentOpened(UiState &ui);

} // namespace Atrum::EntityEditor::EntityEditorClearPrefabSelectionOnSceneOpen