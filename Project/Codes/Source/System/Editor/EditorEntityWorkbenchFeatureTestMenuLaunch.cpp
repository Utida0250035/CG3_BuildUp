#include "System/Editor/EditorEntityWorkbenchFeatureTestMenuLaunch.h"

#include "System/Editor/EditorDebugPlayAreaFeatureVerificationCanonical.h"
#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::Editor {

void QueueFeatureVerificationSceneBuildUp(EntityEditor::UiState &ui) {
  ui.sceneBuildUpLaunchPath =
      DebugPlayAreaFeatureVerificationCanonical::SceneJsonPath();
  ui.requestLaunchSceneBuildUp = true;
}

} // namespace Atrum::Editor