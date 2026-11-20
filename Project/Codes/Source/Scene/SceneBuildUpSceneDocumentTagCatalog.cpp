#include "Scene/SceneBuildUpSceneDocumentTagCatalog.h"

#include "System/Editor/Entity/EntityEditorTagCatalog.h"

namespace Atrum::SceneBuildUpSceneDocumentTagCatalog {

void ApplyAfterJsonLoad(EntityEditor::EditorSceneDocument &document) {
  EntityEditor::MergeTagCatalogFromRegistry(document);
  EntityEditor::SyncAllEntityDefinitionTags(document);
  EntityEditor::SyncTagCatalogFromInstances(document);
}

} // namespace Atrum::SceneBuildUpSceneDocumentTagCatalog