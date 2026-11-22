#pragma once

#include <string>
#include <vector>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::Editor::EditorImportedAssetBatchApplyToSelection {

/// 選択インスタンスへ同じ Imported パスを一括適用。戻り値は成功件数。
int ApplySinglePathToSelectedInstances(EntityEditor::EntityEditor &editor,
                                       const std::string &importedPath);

/// キュー内パスを選択順に割り当て（足りない分は末尾を繰り返し）。
int ApplyPathQueueToSelectedInstances(
    EntityEditor::EntityEditor &editor,
    const std::vector<std::string> &importedPaths);

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
void DrawProjectPanelBatchApplyUi(EntityEditor::EntityEditor &editor);
#endif

} // namespace Atrum::Editor::EditorImportedAssetBatchApplyToSelection