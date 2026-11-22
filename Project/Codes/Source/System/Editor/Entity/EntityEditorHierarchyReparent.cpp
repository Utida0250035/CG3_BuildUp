#include "System/Editor/Entity/EntityEditorHierarchyReparent.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum::EntityEditor::EntityEditorHierarchyReparent {

bool WouldCreateParentCycle(const EditorSceneDocument &scene,
                            const int childIndex,
                            const int newParentIndex) {
  if (newParentIndex < 0) {
    return false;
  }
  if (childIndex == newParentIndex) {
    return true;
  }
  int walk = newParentIndex;
  while (walk >= 0) {
    if (walk == childIndex) {
      return true;
    }
    if (walk >= static_cast<int>(scene.instances.size())) {
      return false;
    }
    walk = scene.instances[static_cast<size_t>(walk)].parentIndex;
  }
  return false;
}

bool Reparent(EditorSceneDocument &scene, const int childIndex,
              const int newParentIndex) {
  if (childIndex < 0 ||
      childIndex >= static_cast<int>(scene.instances.size())) {
    return false;
  }
  if (newParentIndex >= static_cast<int>(scene.instances.size())) {
    return false;
  }
  if (WouldCreateParentCycle(scene, childIndex, newParentIndex)) {
    return false;
  }
  scene.instances[static_cast<size_t>(childIndex)].parentIndex = newParentIndex;
  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorHierarchyReparent