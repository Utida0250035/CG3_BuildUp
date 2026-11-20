#include "Scene/CommandSceneDevelopmentBootApply.h"

#include "Scene/SceneBuildUpLaunchPolicy.h"
#include "Scene/SceneEntityEditor.h"
#include "Scene/SceneTitle.h"

namespace Atrum {

Scene *CreateInitialSceneForDevelopmentBoot(const DevelopmentBootTarget &target) {
  SceneEntityEditor *const entityEditor = SceneEntityEditor::Instance();

  switch (target.kind) {
  case DevelopmentBootKind::SceneBuildUp:
    if (!target.sceneJsonPath.empty()) {
      return SceneBuildUpLaunchPolicy::CreateOwnedBuildUp(target.sceneJsonPath,
                                                         entityEditor);
    }
    break;
  case DevelopmentBootKind::SceneBuildUpPlayRuntime:
    if (!target.sceneJsonPath.empty()) {
      return SceneBuildUpLaunchPolicy::CreateOwnedBuildUp(target.sceneJsonPath,
                                                         SceneTitle::Instance());
    }
    break;
  case DevelopmentBootKind::EntityEditorWithJson:
    if (!target.sceneJsonPath.empty()) {
      entityEditor->SetBootJsonPath(target.sceneJsonPath);
    }
    return entityEditor;
  case DevelopmentBootKind::EntityEditorDefault:
  default:
    break;
  }

  return entityEditor;
}

} // namespace Atrum