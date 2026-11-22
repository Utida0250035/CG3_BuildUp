#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::Editor::Prefab::PrefabAssetOverwriteSaveUi {

/// 一覧で選択中プレハブへ上書き保存ボタン
void DrawOverwriteSelectedPrefabSaveButton(EntityEditor::EntityEditor &editor);

} // namespace Atrum::Editor::Prefab::PrefabAssetOverwriteSaveUi