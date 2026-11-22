#pragma once

namespace Atrum::EntityEditor {
class EntityEditor;
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Editor::EditorImportedAssetProjectPanelList {

/// Imported フォルダ一覧（`ListImportedAssetRelativePaths`）と選択適用 UI。
void DrawImportedAssetsFolderList(EntityEditor::EntityEditor &editor);

} // namespace Atrum::Editor::EditorImportedAssetProjectPanelList

#endif