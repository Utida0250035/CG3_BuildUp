#include "Scene/SceneEntityEditorEnterSceneDocumentLoadPolicy.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum {

SceneEntityEditorEnterSceneLoadPlan PlanEnterSceneDocumentLoad(
    const std::string &pendingBootJsonPath, EntityEditor::EntityEditor &editor,
    const bool hasEnteredSceneEntityEditorBefore) {
  SceneEntityEditorEnterSceneLoadPlan plan{};

  if (!pendingBootJsonPath.empty()) {
    plan.loadFromDisk = true;
    plan.jsonPathToLoad = pendingBootJsonPath;
    return plan;
  }

  if (hasEnteredSceneEntityEditorBefore) {
    plan.loadFromDisk = false;
    return plan;
  }

  const std::string &currentPath = editor.Ui().jsonFilePath;
  if (!currentPath.empty()) {
    plan.loadFromDisk = true;
    plan.jsonPathToLoad = currentPath;
  }

  return plan;
}

} // namespace Atrum