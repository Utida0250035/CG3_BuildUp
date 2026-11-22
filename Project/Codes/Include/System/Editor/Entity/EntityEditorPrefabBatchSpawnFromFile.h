#pragma once

#include <string>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorPrefabBatchSpawnFromFile {

/// 同一プレハブを count 体、X 方向 spacing 間隔で配置。成功した追加数を返す。
int AppendFromPrefabFileBatch(EntityEditor &editor, const std::string &prefabPath,
                              int count, float spacing);

} // namespace Atrum::EntityEditor::EntityEditorPrefabBatchSpawnFromFile