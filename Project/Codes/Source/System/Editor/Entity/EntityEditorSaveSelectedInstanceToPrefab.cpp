#include "System/Editor/Entity/EntityEditorSaveSelectedInstanceToPrefab.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorTagCatalog.h"
#include "System/Json/JsonHandler.h"

namespace Atrum::EntityEditor::EntityEditorSaveSelectedInstanceToPrefab {

bool Save(EntityEditor &editor, const std::string &prefabPath) {
  if (prefabPath.empty()) {
    return false;
  }
  EditorSceneDocument &scene = editor.SceneDocument();
  if (scene.instances.empty()) {
    return false;
  }

  editor.ClampSelectedInstanceIndex();
  const UiState &ui = editor.Ui();
  if (ui.selectedInstanceIndex < 0 ||
      ui.selectedInstanceIndex >= static_cast<int>(scene.instances.size())) {
    return false;
  }

  SceneEntityInstance prefabInstance =
      scene.instances[static_cast<size_t>(ui.selectedInstanceIndex)];
  prefabInstance.parentIndex = -1;
  RebuildEntityDefinitionTagHashes(prefabInstance.definition);
  try {
    Json::JsonHandler::SaveToFile(prefabPath, prefabInstance);
    return true;
  } catch (...) {
    return false;
  }
}

} // namespace Atrum::EntityEditor::EntityEditorSaveSelectedInstanceToPrefab