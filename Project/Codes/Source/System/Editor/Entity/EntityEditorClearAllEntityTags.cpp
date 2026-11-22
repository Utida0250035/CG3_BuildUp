#include "System/Editor/Entity/EntityEditorClearAllEntityTags.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum::EntityEditor::EntityEditorClearAllEntityTags {

void ClearAllTagsOnDefinition(EntityDefinition &definition) {
  definition.tags.clear();
  definition.tagHashes.clear();
}

int ClearAllTagsOnInstanceIndices(EditorSceneDocument &scene,
                                  const std::vector<int> &indices) {
  int cleared = 0;
  for (const int index : indices) {
    if (index < 0 || index >= static_cast<int>(scene.instances.size())) {
      continue;
    }
    ClearAllTagsOnDefinition(
        scene.instances[static_cast<size_t>(index)].definition);
    ++cleared;
  }
  return cleared;
}

} // namespace Atrum::EntityEditor::EntityEditorClearAllEntityTags