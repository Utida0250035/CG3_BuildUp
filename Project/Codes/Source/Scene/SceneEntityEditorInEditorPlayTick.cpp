#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditorInEditorPlayTick.h"

#include "Scene/SceneEntityEditorRuntimeTabFocus.h"
#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::EntityEditor::SceneEntityEditorInEditorPlayTick {

bool Process(EntityEditor &editor,
             const std::function<void()> &rebuildPreviewScene) {
  UiState &ui = editor.Ui();
  if (!ui.requestPlayRuntimeScene) {
    return false;
  }
  ui.requestPlayRuntimeScene = false;
  if (!ui.jsonFilePath.empty()) {
    editor.SaveToJsonPath(ui.jsonFilePath);
  }
  if (rebuildPreviewScene) {
    rebuildPreviewScene();
  }
  ApplyInEditorPlayRuntimeTabFocusOnly(ui);
  return true;
}

} // namespace Atrum::EntityEditor::SceneEntityEditorInEditorPlayTick

#endif