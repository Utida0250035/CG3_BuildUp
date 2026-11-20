#include "Scene/SceneBuildUpSceneDocumentLoad.h"

#include "Camera/StageViewFramingBounds.h"
#include "System/Editor/Scene/EditorSceneRuntime.h"

namespace Atrum::SceneBuildUpSceneDocumentLoad {

bool TryLoad(const std::string &jsonFilePath,
             EntityEditor::EditorSceneDocument &outDocument) {
  return EntityEditor::TryLoadSceneDocument(jsonFilePath, outDocument);
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