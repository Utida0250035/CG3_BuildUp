#include "System/Editor/Entity/EntityEditorPrefabSaveEntityDefinitionBody.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum::EntityEditor::EntityEditorPrefabSaveEntityDefinitionBody {

EntityDefinition BuildEntityDefinitionForPrefabFile(
    const SceneEntityInstance &instance) {
  EntityDefinition definition = instance.definition;
  if (!instance.instanceName.empty()) {
    definition.entityName = instance.instanceName;
  }
  return definition;
}

} // namespace Atrum::EntityEditor::EntityEditorPrefabSaveEntityDefinitionBody