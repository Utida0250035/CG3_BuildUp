#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorScenePlayBuildUpRoute.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum::EntityEditor {

void QueueEditorScenePlayViaBuildUp(EntityEditorUi &ui) {
  ui.requestSaveToDisk = true;
  const std::string playPath =
      ui.jsonFilePath.empty() ? ui.selectedScenePath : ui.jsonFilePath;
  if (!playPath.empty()) {
    Editor::Scene::StageEditorPlayScenePath(playPath);
    ui.sceneBuildUpLaunchPath = playPath;
  } else {
    ui.sceneBuildUpLaunchPath.clear();
  }
  ui.requestLaunchSceneBuildUp = true;
}

} // namespace Atrum::EntityEditor

#endif