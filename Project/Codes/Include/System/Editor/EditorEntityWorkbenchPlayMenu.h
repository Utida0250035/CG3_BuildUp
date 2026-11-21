#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
} // namespace Atrum::EntityEditor

namespace Atrum::Editor::EditorEntityWorkbenchPlayMenu {

/// メインメニューバー「プレイ」（保存＋実行を一本化）。
void DrawMenuItems(EntityEditor::EntityEditor &editor);

} // namespace Atrum::Editor::EditorEntityWorkbenchPlayMenu