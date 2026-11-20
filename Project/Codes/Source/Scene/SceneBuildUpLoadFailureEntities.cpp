#include "Scene/SceneBuildUpLoadFailureEntities.h"

#include "Scene/SceneBuildUpSceneDocumentLoad.h"

namespace Atrum::SceneBuildUpLoadFailureEntities {

std::vector<std::unique_ptr<Entity>>
BuildEntitiesAfterDocumentLoad(
    bool loadSucceeded, const EntityEditor::EditorSceneDocument &document) {
  if (!loadSucceeded) {
    return {};
  }
  return SceneBuildUpSceneDocumentLoad::BuildEntities(document);
}

} // namespace Atrum::SceneBuildUpLoadFailureEntities