#include "System/Editor/Entity/EntityEditorDuplicateSelectedInstance.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor {

namespace {

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

bool DuplicateSelectedInstanceInScene(EntityEditor &editor) {
  EditorSceneDocument &scene = editor.SceneDocument();
  if (scene.instances.empty()) {
    return false;
  }

  editor.ClampSelectedInstanceIndex();
  const UiState &ui = editor.Ui();
  if (ui.selectedInstanceIndex < 0 ||
      ui.selectedInstanceIndex >= static_cast<int>(scene.instances.size())) {
    return false;
  }

  const SceneEntityInstance &source =
      scene.instances[static_cast<size_t>(ui.selectedInstanceIndex)];
  SceneEntityInstance copy = source;
  copy.instanceName =
      MakeUniqueDuplicateInstanceName(scene, source.instanceName);
  copy.definition.entityName = copy.instanceName;

  copy.definition.initialTransform.translate.x += 0.5f;
  copy.definition.initialTransform.translate.z += 0.5f;

  scene.instances.push_back(std::move(copy));
  const int newIndex = static_cast<int>(scene.instances.size()) - 1;

  UiState &mutableUi = editor.Ui();
  mutableUi.selectedInstanceIndex = newIndex;
  mutableUi.selectedInstanceIndices = {newIndex};
  mutableUi.requestApplyPreview = true;
  editor.MarkSceneDocumentDirty();
  return true;
}

} // namespace Atrum::EntityEditor