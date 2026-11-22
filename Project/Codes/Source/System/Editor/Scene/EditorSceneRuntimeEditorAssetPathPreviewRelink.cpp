#include "System/Editor/Scene/EditorSceneRuntimeEditorAssetPathPreviewRelink.h"

#include <algorithm>

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include "System/Editor/Scene/EditorSceneRuntimeBuildWarnings.h"
#include "System/Editor/Scene/EditorSceneRuntimeEditorAssetPathPreviewLink.h"
#include "System/Entity/Entity.h"

namespace Atrum::EntityEditor {

void RelinkEditorAssetPathPreviewsInScene(
    const std::vector<std::unique_ptr<Entity>> &entities,
    const EditorSceneDocument &document,
    EditorSceneRuntimeBuildWarnings *warnings) {
  const size_t count = entities.size();
  if (count == 0 || document.instances.empty()) {
    return;
  }
  const size_t pairs = std::min(count, document.instances.size());
  for (size_t i = 0; i < pairs; ++i) {
    Entity *entity = entities[i].get();
    if (entity == nullptr) {
      continue;
    }
    const std::string &label = document.instances[i].instanceName;
    EditorSceneRuntimeEditorAssetPathPreviewLink::Apply(*entity, label,
                                                      warnings);
  }
}

} // namespace Atrum::EntityEditor