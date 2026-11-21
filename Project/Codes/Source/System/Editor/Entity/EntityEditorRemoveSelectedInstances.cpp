#include "System/Editor/Entity/EntityEditorRemoveSelectedInstances.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

#include <algorithm>
#include <unordered_set>

namespace Atrum::EntityEditor::EntityEditorRemoveSelectedInstances {

namespace {

std::unordered_set<int>
CollectRemovalIndices(const EntityEditor &editor) {
  const EditorSceneDocument &scene = editor.SceneDocument();
  const UiState &ui = editor.Ui();
  std::unordered_set<int> removal;

  if (!ui.selectedInstanceIndices.empty()) {
    for (const int index : ui.selectedInstanceIndices) {
      if (index >= 0 && index < static_cast<int>(scene.instances.size())) {
        removal.insert(index);
      }
    }
  }
  if (removal.empty() && ui.selectedInstanceIndex >= 0 &&
      ui.selectedInstanceIndex < static_cast<int>(scene.instances.size())) {
    removal.insert(ui.selectedInstanceIndex);
  }
  return removal;
}

int ResolveParentIndexAfterRemoval(
    const EditorSceneDocument &originalScene,
    const std::vector<int> &oldToNew,
    const std::unordered_set<int> &removal, int oldParentIndex) {
  int current = oldParentIndex;
  while (current >= 0) {
    if (!removal.contains(current)) {
      if (current >= static_cast<int>(oldToNew.size())) {
        return -1;
      }
      const int mapped = oldToNew[static_cast<size_t>(current)];
      return mapped >= 0 ? mapped : -1;
    }
    if (current >= static_cast<int>(originalScene.instances.size())) {
      return -1;
    }
    current = originalScene.instances[static_cast<size_t>(current)].parentIndex;
  }
  return -1;
}

} // namespace

void RemoveFromScene(EntityEditor &editor) {
  EditorSceneDocument &scene = editor.SceneDocument();
  UiState &ui = editor.Ui();
  if (scene.instances.size() <= 1) {
    return;
  }

  editor.ClampSelectedInstanceIndex();
  std::unordered_set<int> removal = CollectRemovalIndices(editor);
  if (removal.empty()) {
    return;
  }

  if (removal.size() >= scene.instances.size()) {
    removal.erase(ui.selectedInstanceIndex);
  }
  if (removal.empty()) {
    return;
  }

  const EditorSceneDocument originalScene = scene;
  const size_t instanceCount = originalScene.instances.size();
  std::vector<int> oldToNew(instanceCount, -1);
  std::vector<SceneEntityInstance> kept;
  kept.reserve(instanceCount);

  for (size_t i = 0; i < instanceCount; ++i) {
    if (removal.contains(static_cast<int>(i))) {
      continue;
    }
    oldToNew[i] = static_cast<int>(kept.size());
    kept.push_back(originalScene.instances[i]);
  }

  for (SceneEntityInstance &instance : kept) {
    if (instance.parentIndex < 0) {
      continue;
    }
    instance.parentIndex = ResolveParentIndexAfterRemoval(
        originalScene, oldToNew, removal, instance.parentIndex);
  }

  scene.instances = std::move(kept);

  std::vector<int> newSelection;
  newSelection.reserve(ui.selectedInstanceIndices.size());
  for (const int index : ui.selectedInstanceIndices) {
    if (index < 0 || index >= static_cast<int>(instanceCount) ||
        removal.contains(index)) {
      continue;
    }
    const int mapped = oldToNew[static_cast<size_t>(index)];
    if (mapped >= 0) {
      newSelection.push_back(mapped);
    }
  }
  if (newSelection.empty() && !scene.instances.empty()) {
    newSelection.push_back(0);
  }
  std::sort(newSelection.begin(), newSelection.end());
  newSelection.erase(std::unique(newSelection.begin(), newSelection.end()),
                     newSelection.end());
  ui.selectedInstanceIndices = newSelection;
  ui.selectedInstanceIndex =
      newSelection.empty() ? 0 : newSelection.front();
  editor.ClampSelectedInstanceIndex();
  EntityEditorAuthoringDocumentEditNotify::Notify(editor);
}

} // namespace Atrum::EntityEditor::EntityEditorRemoveSelectedInstances