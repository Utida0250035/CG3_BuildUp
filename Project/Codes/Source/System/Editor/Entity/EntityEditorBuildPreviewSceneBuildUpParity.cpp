#include "System/Editor/Entity/EntityEditorBuildPreviewSceneBuildUpParity.h"

#include "Scene/SceneBuildUpSceneDocumentLoad.h"
#include "Scene/SceneBuildUpSceneDocumentTagCatalog.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Scene/EditorSceneAsset.h"

namespace Atrum::EntityEditor::EntityEditorBuildPreviewSceneBuildUpParity {

std::vector<std::unique_ptr<Entity>>
Build(const EditorSceneDocument &document, EntityEditorUi &ui,
      std::string *importStatusMessageOut) {
  EditorSceneDocument buildDocument = document;
  SceneBuildUpSceneDocumentTagCatalog::ApplyAfterJsonLoad(buildDocument);

  std::vector<std::unique_ptr<Entity>> entities =
      SceneBuildUpSceneDocumentLoad::BuildEntities(buildDocument);
  if (importStatusMessageOut != nullptr) {
    importStatusMessageOut->clear();
  }

  const std::string playPath =
      ui.jsonFilePath.empty() ? ui.selectedScenePath : ui.jsonFilePath;
  if (!playPath.empty()) {
    ui.sceneBuildUpLaunchPath = playPath;
    Atrum::Editor::Scene::StageEditorPlayScenePath(
        playPath, Atrum::Editor::Scene::EditorScenePlayStagingKind::
                      InEditorRuntimePreview);
  }

  return entities;
}

} // namespace Atrum::EntityEditor::EntityEditorBuildPreviewSceneBuildUpParity