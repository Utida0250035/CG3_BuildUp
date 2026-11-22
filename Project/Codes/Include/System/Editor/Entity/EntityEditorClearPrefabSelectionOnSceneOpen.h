#pragma once

namespace Atrum::Editor::Entity {
struct UiState;
}

namespace Atrum::EntityEditor::EntityEditorClearPrefabSelectionOnSceneOpen {

/// シーンドキュメントを開いた直後にプレハブ一覧選択をリセットする
void OnSceneDocumentOpened(EntityEditor::UiState &ui);

} // namespace Atrum::EntityEditor::EntityEditorClearPrefabSelectionOnSceneOpen