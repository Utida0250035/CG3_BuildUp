#include "Scene/SceneEntityEditorRuntimeViewCameraDraw.h"

#include "Scene/SceneEntityEditor.h"
#include "Scene/SceneEntityEditorPreviewCameraSetup.h"
#include "System/Editor/EditorFrame.h"

namespace Atrum::SceneEntityEditorRuntimeViewCameraDraw {

void PrepareSceneViewCameraForDraw(
    SceneEntityEditor &editor, const Editor::SceneViewTab viewTab,
    const std::vector<std::unique_ptr<Entity>> &previewEntities,
    const bool runtimeFunctionalViewDraw) {
  if (viewTab != Editor::SceneViewTab::Runtime) {
    return;
  }
  if (runtimeFunctionalViewDraw) {
    editor.PrepareCamerasForDraw();
    return;
  }
  if (SceneEntityEditorPreviewCameraSetup::ApplyPrimaryFromPreviewEntities(
          previewEntities, editor.AllowDebugCameraControl())) {
    return;
  }
  editor.PrepareCamerasForDraw();
}

} // namespace Atrum::SceneEntityEditorRuntimeViewCameraDraw