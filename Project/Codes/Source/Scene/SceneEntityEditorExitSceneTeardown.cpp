#include "Scene/SceneEntityEditorExitSceneTeardown.h"

#include "System/Editor/Entity/EntityEditor.h"
#include "System/Entity/Entity.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/EditorFrame.h"
#endif

namespace Atrum::EntityEditor::SceneEntityEditorExitSceneTeardown {

void ReleasePreviewEntities(
    std::vector<std::unique_ptr<Entity>> &previewEntities) {
  for (std::unique_ptr<Entity> &entity : previewEntities) {
    if (entity) {
      entity->Finalize();
    }
  }
  previewEntities.clear();
}

void RestoreSceneViewTabAfterBuildUpExit(EntityEditor &editor) {
  UiState &ui = editor.Ui();
  ui.requestFocusRuntimeSceneView = false;
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  Editor::EditorFrame::Instance().SetSceneViewTab(Editor::SceneViewTab::Edit);
#endif
}

void ResetEditorTransientPlayUi(EntityEditor &editor) {
  UiState &ui = editor.Ui();
  ui.requestPlayRuntimeScene = false;
  ui.requestLaunchSceneBuildUp = false;
  ui.sceneBuildUpLaunchPath.clear();
  ui.requestFocusRuntimeSceneView = false;
  ui.requestOpenParticleEditor = false;
  ui.requestApplyPreview = false;
  ui.requestResetPreview = false;
  ui.previewSimulationEnabled = false;
  ui.interactionMode = EditorInteractionMode::Edit;
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  Editor::EditorFrame::Instance().SetSceneViewTab(Editor::SceneViewTab::Edit);
#endif
}

} // namespace Atrum::EntityEditor::SceneEntityEditorExitSceneTeardown