#pragma once

#include <string>
#include <vector>

namespace Atrum::Editor::Entity {
class EntityEditor;
}

namespace Atrum::Editor::Prefab::PrefabAssetProjectPanelList {

/// プレハブ JSON 一覧・選択・削除・改名 UI
void DrawPrefabJsonFileList(EntityEditor::EntityEditor &editor,
                            const std::vector<std::string> &prefabPaths);

} // namespace Atrum::Editor::Prefab::PrefabAssetProjectPanelList