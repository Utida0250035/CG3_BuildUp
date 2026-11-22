#pragma once

#include <string>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorSceneDocumentLoadFromPath {

/// ディスクからシーン JSON を読み込む。失敗時は statusMessage に理由を書く。
bool TryLoad(EntityEditor &editor, const std::string &jsonFilePath,
             std::string &statusMessage);

} // namespace Atrum::EntityEditor::EntityEditorSceneDocumentLoadFromPath