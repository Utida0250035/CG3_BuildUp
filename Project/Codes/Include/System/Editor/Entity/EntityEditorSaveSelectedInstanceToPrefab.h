#pragma once

#include <string>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorSaveSelectedInstanceToPrefab {

/// 選択インスタンスを SceneEntityInstance 形式でプレハブ JSON へ保存する。
bool Save(EntityEditor &editor, const std::string &prefabPath);

} // namespace Atrum::EntityEditor::EntityEditorSaveSelectedInstanceToPrefab