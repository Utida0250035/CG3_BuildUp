#include "System/Editor/Entity/EntityEditorSceneDocumentLoadFromPath.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"

#include <filesystem>

namespace Atrum::EntityEditor::EntityEditorSceneDocumentLoadFromPath {

bool TryLoad(EntityEditor &editor, const std::string &jsonFilePath,
             std::string &statusMessage) {
  namespace fs = std::filesystem;

  if (jsonFilePath.empty()) {
    statusMessage = "シーン JSON のパスが空です";
    return false;
  }

  if (!fs::exists(jsonFilePath)) {
    statusMessage =
        "シーン JSON が見つかりません。新規空シーンで開始します: " +
        jsonFilePath;
    editor.ApplyEmptySceneForMissingJsonPath(jsonFilePath);
    return false;
  }

  try {
    EditorSceneDocument document{};
    if (!TryLoadSceneDocument(jsonFilePath, document)) {
      statusMessage = "シーン JSON を開けません: " + jsonFilePath;
      return false;
    }
    editor.ApplySceneDocumentLoadedFromDisk(std::move(document));
    statusMessage.clear();
    return true;
  } catch (const std::exception &ex) {
    statusMessage = std::string("シーン JSON の読み込みに失敗しました: ") +
                    ex.what();
    return false;
  }
}

} // namespace Atrum::EntityEditor::EntityEditorSceneDocumentLoadFromPath