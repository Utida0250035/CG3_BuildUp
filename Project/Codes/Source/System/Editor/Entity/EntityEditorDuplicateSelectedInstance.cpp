#include "System/Editor/Entity/EntityEditorDuplicateSelectedInstance.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Editor/Entity/EntityEditorAuthoringDocumentEditNotify.h"

#include <algorithm>

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

std::vector<int> CollectDuplicateSourceIndices(const EntityEditor &editor) {
  const EditorSceneDocument &scene = editor.SceneDocument();
  const UiState &ui = editor.Ui();
  std::vector<int> sources = ui.selectedInstanceIndices;
  if (sources.empty() && ui.selectedInstanceIndex >= 0) {
    sources.push_back(ui.selectedInstanceIndex);
  }
  std::sort(sources.begin(), sources.end());
  sources.erase(std::unique(sources.begin(), sources.end()), sources.end());
  sources.erase(std::remove_if(sources.begin(), sources.end(),
                               [&scene](const int index) {
                                 return index < 0 ||
                                        index >= static_cast<int>(
                                                    scene.instances.size());
                               }),
              sources.end());
  return sources;
}

} // namespace

bool DuplicateSelectedInstanceInScene(EntityEditor &editor) {
  EditorSceneDocument &scene = editor.SceneDocument();
  if (scene.instances.empty()) {
    return false;
  }

  editor.ClampSelectedInstanceIndex();
  const std::vector<int> sources = CollectDuplicateSourceIndices(editor);
  if (sources.empty()) {
    return false;
  }

  std::vector<int> newSelection;
  newSelection.reserve(sources.size());
  float offsetStep = 0.5f;

  for (size_t slot = 0; slot < sources.size(); ++slot) {
    const int sourceIndex = sources[slot];
    const SceneEntityInstance &source =
        scene.instances[static_cast<size_t>(sourceIndex)];
    SceneEntityInstance copy = source;
    copy.instanceName =
        MakeUniqueDuplicateInstanceName(scene, source.instanceName);
    copy.definition.entityName = copy.instanceName;
    const float offset = offsetStep * (static_cast<float>(slot) + 1.0f);
    copy.definition.initialTransform.translate.x += offset;
    copy.definition.initialTransform.translate.z += offset;

    scene.instances.push_back(std::move(copy));
    newSelection.push_back(static_cast<int>(scene.instances.size()) - 1);
  }

  UiState &mutableUi = editor.Ui();
  mutableUi.selectedInstanceIndices = newSelection;
  mutableUi.selectedInstanceIndex = newSelection.front();
  editor.ClampSelectedInstanceIndex();
  EntityEditorAuthoringDocumentEditNotify::Notify(editor);
  return true;
}

} // namespace Atrum::EntityEditor