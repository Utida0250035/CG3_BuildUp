#include "System/Editor/Entity/EntityEditorHierarchyInstanceReorder.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"

#include <vector>

namespace Atrum::EntityEditor::EntityEditorHierarchyInstanceReorder {

bool MoveAfterSibling(EditorSceneDocument &scene, const int fromIndex,
                      const int afterIndex, int *outNewFromIndex) {
  const size_t count = scene.instances.size();
  if (fromIndex == afterIndex || fromIndex < 0 ||
      afterIndex < 0 || static_cast<size_t>(fromIndex) >= count ||
      static_cast<size_t>(afterIndex) >= count) {
    return false;
  }
  const int sharedParent =
      scene.instances[static_cast<size_t>(afterIndex)].parentIndex;
  scene.instances[static_cast<size_t>(fromIndex)].parentIndex = sharedParent;

  std::vector<int> order;
  order.reserve(count);
  for (size_t index = 0; index < count; ++index) {
    order.push_back(static_cast<int>(index));
  }

  order.erase(order.begin() + fromIndex);
  int insertAt = afterIndex;
  if (fromIndex < afterIndex) {
    --insertAt;
  }
  ++insertAt;
  if (insertAt > static_cast<int>(order.size())) {
    insertAt = static_cast<int>(order.size());
  }
  order.insert(order.begin() + insertAt, fromIndex);

  std::vector<int> oldToNew(count, -1);
  std::vector<SceneEntityInstance> reordered;
  reordered.reserve(count);
  for (size_t newPos = 0; newPos < count; ++newPos) {
    const int oldIndex = order[newPos];
    oldToNew[static_cast<size_t>(oldIndex)] = static_cast<int>(newPos);
    reordered.push_back(std::move(scene.instances[static_cast<size_t>(oldIndex)]));
  }

  for (SceneEntityInstance &instance : reordered) {
    if (instance.parentIndex < 0) {
      continue;
    }
    const size_t parentPos = static_cast<size_t>(instance.parentIndex);
    if (parentPos >= oldToNew.size() || oldToNew[parentPos] < 0) {
      instance.parentIndex = -1;
      continue;
    }
    instance.parentIndex = oldToNew[parentPos];
  }

  scene.instances = std::move(reordered);
  if (outNewFromIndex != nullptr) {
    *outNewFromIndex = oldToNew[static_cast<size_t>(fromIndex)];
  }
  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorHierarchyInstanceReorder