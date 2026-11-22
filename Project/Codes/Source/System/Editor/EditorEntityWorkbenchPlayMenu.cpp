#include "System/Editor/EditorEntityWorkbenchPlayMenu.h"

#include "Scene/SceneEntityEditorInEditorPlayQueue.h"
#include "Scene/SceneEntityEditorScenePlayBuildUpRoute.h"
#include "System/Editor/EditorFrame.h"
#include "System/Editor/EditorSceneRunStatus.h"
#include "System/Editor/EditorSceneViewPanel.h"
#include "System/Editor/Entity/EntityEditor.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include <imgui/imgui.h>
#endif

namespace Atrum::Editor::EditorEntityWorkbenchPlayMenu {

void DrawMenuItems(EntityEditor::EntityEditor &editor) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  EntityEditor::UiState &ui = editor.Ui();
  if (!ImGui::BeginMenu("プレイ")) {
    return;
  }

  if (ImGui::MenuItem("保存して実行（ランタイムタブ）")) {
    Atrum::EntityEditor::QueueInEditorPlayRuntimeTabOnly(ui);
    SetEntityEditorSceneMode(ui, true);
    EditorFrame::Instance().SetSceneViewTab(SceneViewTab::Runtime);
  }
  if (ImGui::MenuItem("保存して BuildUp で実行")) {
    Atrum::EntityEditor::QueueEditorScenePlayViaBuildUp(ui);
  }
  ImGui::Separator();
  ImGui::TextDisabled("以下はディスク保存なし");
  if (ImGui::MenuItem("プレビューシミュレーションのみ", nullptr,
                      ui.interactionMode ==
                          EntityEditor::EditorInteractionMode::Play)) {
    SetEntityEditorSceneMode(ui, true);
  }

  ImGui::EndMenu();
#endif
}

} // namespace Atrum::Editor::EditorEntityWorkbenchPlayMenu