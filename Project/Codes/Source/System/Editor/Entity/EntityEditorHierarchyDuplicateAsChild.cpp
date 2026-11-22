#include "System/Editor/Entity/EntityEditorHierarchyDuplicateAsChild.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"
namespace Atrum::EntityEditor::EntityEditorHierarchyDuplicateAsChild {

namespace {

std::string MakeUniqueDuplicateInstanceName(
    const EditorSceneDocument &scene, const std::string &baseName);

std::string MakeUniqueDuplicateInstanceName(
    const EditorSceneDocument &scene, const std::string &baseName) {
  auto nameExists = [&scene](const std::string &name) {
    for (const SceneEntityInstance &instance : scene.instances) {
      if (instance.instanceName == name) {
        return true;
      }
    }
    return false;
  };

  std::string candidate = baseName + "_Copy";
  if (!nameExists(candidate)) {
    return candidate;
  }
  for (int suffix = 2; suffix < 100000; ++suffix) {
    candidate = baseName + "_Copy" + std::to_string(suffix);
    if (!nameExists(candidate)) {
      return candidate;
    }
  }
  return baseName + "_Copy_overflow";
}

} // namespace

bool DuplicateUnderParent(EntityEditor &editor, const int sourceIndex,
                          const int newParentIndex) {
  EditorSceneDocument &scene = editor.SceneDocument();
  if (sourceIndex < 0 ||
      sourceIndex >= static_cast<int>(scene.instances.size())) {
    return false;
  }
  if (newParentIndex >= static_cast<int>(scene.instances.size())) {
    return false;
  }

  const SceneEntityInstance &source =
      scene.instances[static_cast<size_t>(sourceIndex)];
  SceneEntityInstance copy = source;
  copy.instanceName = MakeUniqueDuplicateInstanceName(scene, source.instanceName);
  copy.definition.entityName = copy.instanceName;
  copy.parentIndex = newParentIndex;
  copy.definition.initialTransform.translate.x += 0.25f;
  copy.definition.initialTransform.translate.z += 0.25f;

  scene.instances.push_back(std::move(copy));
  const int newIndex = static_cast<int>(scene.instances.size()) - 1;

  UiState &ui = editor.Ui();
  ui.selectedInstanceIndex = newIndex;
  ui.selectedInstanceIndices = {newIndex};
  editor.ClampSelectedInstanceIndex();
  EntityEditorAuthoringDocumentEditNotify::Notify(editor);
  return true;
}

} // namespace Atrum::EntityEditor::EntityEditorHierarchyDuplicateAsChild