#include "Scene/SceneBuildUpEnterSceneEditorUiPolicy.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "Scene/SceneEntityEditor.h"
#endif

namespace Atrum::SceneBuildUpEnterSceneEditorUiPolicy {

void ApplyOnEnterScene() {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  SceneEntityEditor::ClearStaleRuntimeSceneViewFocusRequest();
#endif
}

} // namespace Atrum::SceneBuildUpEnterSceneEditorUiPolicy