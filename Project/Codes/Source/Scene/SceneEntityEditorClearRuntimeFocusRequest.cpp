#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditor.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum {

void SceneEntityEditor::ClearStaleRuntimeSceneViewFocusRequest() {
  SceneEntityEditor *const editorScene = Instance();
  if (editorScene == nullptr || editorScene->editor_ == nullptr) {
    return;
  }
  editorScene->editor_->Ui().requestFocusRuntimeSceneView = false;
}

} // namespace Atrum

#endif