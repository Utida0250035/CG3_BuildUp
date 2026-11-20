#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorInEditorPlayQueue.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum::EntityEditor {

void QueueInEditorPlayRuntimeTabOnly(EntityEditorUi &ui) {
  ui.requestSaveToDisk = true;
  const std::string playPath =
      ui.jsonFilePath.empty() ? ui.selectedScenePath : ui.jsonFilePath;
  if (!playPath.empty()) {
    Editor::Scene::StageEditorPlayScenePath(playPath);
  }
  ui.requestPlayRuntimeScene = true;
}

} // namespace Atrum::EntityEditor

#endif