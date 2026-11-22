#include "Scene/SceneBuildUpSceneDocumentLoad.h"

#include "Camera/StageViewFramingBounds.h"
#include "Scene/SceneBuildUpSceneDocumentTagCatalog.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"
#include "System/Entity/Entity.h"

namespace Atrum::SceneBuildUpSceneDocumentLoad {

bool TryLoad(const std::string &jsonFilePath,
             EntityEditor::EditorSceneDocument &outDocument) {
  if (!EntityEditor::TryLoadSceneDocument(jsonFilePath, outDocument)) {
    return false;
  }
  SceneBuildUpSceneDocumentTagCatalog::ApplyAfterJsonLoad(outDocument);
  return true;
}

std::vector<std::unique_ptr<Entity>>
BuildEntities(const EntityEditor::EditorSceneDocument &document) {
  return EntityEditor::BuildEntitiesFromSceneDocument(document);
}

StageViewFramingBounds
ResolveStageViewFraming(const EntityEditor::EditorSceneDocument &document) {
  return EntityEditor::ResolveStageViewFramingBounds(document);
}

} // namespace Atrum::SceneBuildUpSceneDocumentLoad