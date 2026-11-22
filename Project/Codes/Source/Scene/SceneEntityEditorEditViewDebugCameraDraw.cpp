#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorEditViewDebugCameraDraw.h"

#include "Scene/SceneEntityEditor.h"
#include "System/Editor/EditorFrame.h"
#include "System/Entity/Components/CmpCamera.h"

namespace Atrum {

void SceneEntityEditor::ApplyEditViewHoveredDebugCameraBeforeDraw() {
  if (!Editor::EditorFrame::Instance().EditViewHovered()) {
    return;
  }
  if (CmpCamera *cmp = cameraRig_.FindPrimaryCmpCamera(this)) {
    /// GameOrder 経路は AllowDebugCameraControl=false。Edit ホバーだけ入力を有効化。
    cmp->PrepareBeforeDraw(true);
  }
}

namespace SceneEntityEditorEditViewDebugCameraDraw {

void ApplyHoveredEditViewCameraInput(SceneEntityEditor &editor) {
  editor.ApplyEditViewHoveredDebugCameraBeforeDraw();
}

} // namespace SceneEntityEditorEditViewDebugCameraDraw

} // namespace Atrum

#endif