#include "Scene/SceneBuildUpLoadFailureEntities.h"

#include "Scene/SceneBuildUpSceneDocumentLoad.h"
#include "System/Entity/Entity.h"

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