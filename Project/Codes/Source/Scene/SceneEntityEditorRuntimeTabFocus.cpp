#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorRuntimeTabFocus.h"

#include "System/Editor/EditorFrame.h"
#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor {

void ApplyInEditorPlayRuntimeTabFocusOnly(UiState &ui) {
  Editor::EditorFrame::Instance().SetSceneViewTab(Editor::SceneViewTab::Runtime);
  ui.requestFocusRuntimeSceneView = true;
}

} // namespace Atrum::EntityEditor

#endif