#include "Scene/SceneBuildUpExitSceneEditorUiRestore.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneEntityEditor.h"
#endif

namespace Atrum::SceneBuildUpExitSceneEditorUiRestore {

void ApplyOnExitScene() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  SceneEntityEditor::RestoreUiAfterSceneBuildUpExit();
#endif
}

} // namespace Atrum::SceneBuildUpExitSceneEditorUiRestore