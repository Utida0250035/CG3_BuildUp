#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "Scene/SceneEntityEditor.h"

#include "Scene/SceneEntityEditorExitSceneTeardown.h"
#include "System/Editor/EditorFrame.h"
#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum {

void SceneEntityEditor::RestoreUiAfterSceneBuildUpExit() {
  SceneEntityEditor *const editorScene = Instance();
  if (editorScene == nullptr) {
    return;
  }
  if (editorScene->editor_) {
    EntityEditor::SceneEntityEditorExitSceneTeardown::
        RestoreSceneViewTabAfterBuildUpExit(*editorScene->editor_);
    return;
  }
  Editor::EditorFrame::Instance().SetSceneViewTab(Editor::SceneViewTab::Edit);
}

} // namespace Atrum

#endif