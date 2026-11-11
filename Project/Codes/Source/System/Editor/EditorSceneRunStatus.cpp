#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorSceneRunStatus.h"

#include "ForDebug/ImGui.h"
#include "System/Editor/EditorSceneViewPanel.h"

namespace Atrum::Editor {

namespace {

void FocusSceneViewWindow(const char *windowTitle) {
  ImGui::SetWindowFocus(windowTitle);
}

void SelectSceneViewTab(const SceneViewTab tab) {
  EditorFrame::Instance().SetSceneViewTab(tab);
  switch (tab) {
  case SceneViewTab::Edit:
    FocusSceneViewWindow(kEditSceneViewTitle);
    break;
  case SceneViewTab::Game:
    FocusSceneViewWindow(kGameSceneViewTitle);
    break;
  case SceneViewTab::Runtime:
    FocusSceneViewWindow(kRuntimeSceneViewTitle);
    break;
  }
}

bool IsEntityEditorSimulationTicking(const EntityEditor::UiState &ui,
                                     const SceneViewTab viewTab) {
  if (ui.interactionMode != EntityEditor::EditorInteractionMode::Play ||
      !ui.previewSimulationEnabled) {
    return false;
  }
  return viewTab == SceneViewTab::Runtime || viewTab == SceneViewTab::Edit;
}

} // namespace

const char *EditorSceneRunPhaseLabel(const EditorSceneRunPhase phase) {
  switch (phase) {
  case EditorSceneRunPhase::Waiting:
    return "待機中";
  case EditorSceneRunPhase::Paused:
    return "一時停止中";
  case EditorSceneRunPhase::Running:
    return "実行中";
  }
  return "待機中";
}

EditorSceneRunPhase QueryEntityEditorSceneRunPhase(
    const EntityEditor::UiState &ui, const SceneViewTab viewTab,
    const bool sceneTransitionActive, const bool playRequestPending) {
  if (sceneTransitionActive || playRequestPending) {
    return EditorSceneRunPhase::Waiting;
  }

  if (ui.interactionMode == EntityEditor::EditorInteractionMode::Edit) {
    return EditorSceneRunPhase::Waiting;
  }

  if (!ui.previewSimulationEnabled) {
    return EditorSceneRunPhase::Paused;
  }

  if (IsEntityEditorSimulationTicking(ui, viewTab)) {
    return EditorSceneRunPhase::Running;
  }

  return EditorSceneRunPhase::Paused;
}

void DrawEditorSceneRunPhaseBadge(const EditorSceneRunPhase phase) {
  ImVec4 color{};
  switch (phase) {
  case EditorSceneRunPhase::Waiting:
    color = ImVec4{0.95f, 0.78f, 0.28f, 1.0f};
    break;
  case EditorSceneRunPhase::Paused:
    color = ImVec4{0.55f, 0.58f, 0.62f, 1.0f};
    break;
  case EditorSceneRunPhase::Running:
    color = ImVec4{0.35f, 0.88f, 0.42f, 1.0f};
    break;
  }
  ImGui::TextColored(color, "状態: %s", EditorSceneRunPhaseLabel(phase));
}

void SetEntityEditorSceneMode(EntityEditor::UiState &ui, const bool wantRun) {
  if (wantRun) {
    ui.interactionMode = EntityEditor::EditorInteractionMode::Play;
    ui.previewSimulationEnabled = true;
    ui.requestFocusRuntimeSceneView = true;
  } else {
    ui.interactionMode = EntityEditor::EditorInteractionMode::Edit;
    ui.previewSimulationEnabled = false;
  }
}

void DrawEntityEditorSceneModeToolbar(EntityEditor::EntityEditor &editor,
                                      const EditorSceneRunPhase runPhase) {
  EntityEditor::UiState &ui = editor.Ui();
  const bool isScene =
      ui.interactionMode == EntityEditor::EditorInteractionMode::Edit;
  const bool isRun =
      ui.interactionMode == EntityEditor::EditorInteractionMode::Play;

  if (ImGui::RadioButton("シーン", isScene)) {
    SetEntityEditorSceneMode(ui, false);
    SelectSceneViewTab(SceneViewTab::Edit);
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("実行", isRun)) {
    SetEntityEditorSceneMode(ui, true);
  }
  ImGui::SameLine();
  DrawEditorSceneRunPhaseBadge(runPhase);
}

} // namespace Atrum::Editor

#endif
