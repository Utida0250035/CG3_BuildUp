#include "Scene/CommandSceneDevelopmentEditorBootPolicy.h"

#include "Scene/SceneEntityEditor.h"

namespace Atrum {

Scene *ApplyDevelopmentEditorBootScene(const DevelopmentBootTarget &target) {
  SceneEntityEditor *const entityEditor = SceneEntityEditor::Instance();
  if (!target.sceneJsonPath.empty()) {
    entityEditor->SetBootJsonPath(target.sceneJsonPath);
  }
  return entityEditor;
}

} // namespace Atrum
