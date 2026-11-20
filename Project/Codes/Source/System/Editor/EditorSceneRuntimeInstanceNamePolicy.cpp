#include "System/Editor/EditorSceneRuntimeInstanceNamePolicy.h"

#include "System/Entity/Entity.h"

namespace Atrum::EntityEditor {

void ApplySceneInstanceName(Entity &entity, const SceneEntityInstance &instance) {
  if (!instance.instanceName.empty()) {
    entity.SetHash(instance.instanceName);
    return;
  }
  if (!instance.definition.entityName.empty()) {
    entity.SetHash(instance.definition.entityName);
    return;
  }
  entity.SetHash("Entity");
}

} // namespace Atrum::EntityEditor