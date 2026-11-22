#pragma once

#include <string>
#include <vector>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorDroppedImportSpawnSceneInstances {

/// Imported へコピー済みの相対パスごとにシーンへインスタンス＋Draw/Data を追加する。
int AppendFromImportedAssetPaths(EntityEditor &editor,
                                 const std::vector<std::string> &importedPaths);

} // namespace Atrum::EntityEditor::EntityEditorDroppedImportSpawnSceneInstances