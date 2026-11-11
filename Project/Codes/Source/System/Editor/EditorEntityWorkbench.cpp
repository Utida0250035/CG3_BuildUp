#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorEntityWorkbench.h"

#include "ForDebug/ImGui.h"
#include "ForDebug/AudioStreamingDebugImGui.h"
#include "System/Editor/EditorCodeTemplateScaffold.h"
#include <imgui/imgui_internal.h>
#include "System/Editor/EditorFrame.h"
#include "System/Editor/EditorSceneRunStatus.h"
#include "System/Editor/EditorSceneViewPanel.h"
#include "System/Editor/Entity/EntityEditor.h"
#include "Scene/CommandChangeScene.h"
#include "System/Editor/Prefab/PrefabAsset.h"

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

void DrawSceneEditRunToggle(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();
  const bool isScene =
      ui.interactionMode == EntityEditor::EditorInteractionMode::Edit;
  const bool isRun =
      ui.interactionMode == EntityEditor::EditorInteractionMode::Play;

  ImGui::Separator();
  if (ImGui::RadioButton("シーン", isScene)) {
    SetEntityEditorSceneMode(ui, false);
    SelectSceneViewTab(SceneViewTab::Edit);
  }
  ImGui::SameLine();
  if (ImGui::RadioButton("実行", isRun)) {
    SetEntityEditorSceneMode(ui, true);
  }
}

void CycleSceneViewTab() {
  SceneViewTab tab = EditorFrame::Instance().SceneViewTabSelection();
  switch (tab) {
  case SceneViewTab::Edit:
    tab = SceneViewTab::Game;
    break;
  case SceneViewTab::Game:
    tab = SceneViewTab::Runtime;
    break;
  case SceneViewTab::Runtime:
    tab = SceneViewTab::Edit;
    break;
  }
  SelectSceneViewTab(tab);
}

void DrawMainMenuBar(EntityEditor::EntityEditor &editor) {
  EntityEditor::UiState &ui = editor.Ui();

  if (!ImGui::BeginMainMenuBar()) {
    return;
  }

  if (ImGui::BeginMenu("ファイル")) {
    if (ImGui::MenuItem("再読み込み")) {
      ui.requestReloadFromDisk = true;
    }
    if (ImGui::MenuItem("保存")) {
      ui.requestSaveToDisk = true;
    }
    if (ImGui::MenuItem("新規シーン")) {
      editor.CreateNewSceneFile();
    }
    const std::string deleteSceneTarget =
        !ui.selectedScenePath.empty() ? ui.selectedScenePath : ui.jsonFilePath;
    if (ImGui::MenuItem("シーンを削除...", nullptr, false,
                         !deleteSceneTarget.empty())) {
      ui.pendingDeleteScenePath = deleteSceneTarget;
      ui.requestDeleteSceneConfirm = true;
    }
    if (ImGui::MenuItem("シーンを実行")) {
      ui.requestSaveToDisk = true;
      ui.requestPlayRuntimeScene = true;
    }
    if (ImGui::MenuItem("プレビューに適用")) {
      ui.requestApplyPreview = true;
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("モード")) {
    const bool isEdit =
        ui.interactionMode == EntityEditor::EditorInteractionMode::Edit;
    const bool isPlay =
        ui.interactionMode == EntityEditor::EditorInteractionMode::Play;
    if (ImGui::MenuItem("シーン（編集）", nullptr, isEdit)) {
      SetEntityEditorSceneMode(ui, false);
      SelectSceneViewTab(SceneViewTab::Edit);
    }
    if (ImGui::MenuItem("実行（シミュレーション）", nullptr, isPlay)) {
      SetEntityEditorSceneMode(ui, true);
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("プレビュー")) {
    if (ImGui::MenuItem("リセット")) {
      ui.requestResetPreview = true;
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("エディタ")) {
    if (ImGui::MenuItem("パーティクルエディタ")) {
      ui.requestOpenParticleEditor = true;
    }
    if (ImGui::MenuItem("コードテンプレ")) {
      ImGui::SetWindowFocus("コードテンプレ");
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("デバッグ")) {
    bool showStreamingMemory = Atrum::Debug::IsStreamingMemoryDebugVisible();
    if (ImGui::MenuItem("ストリーミングメモリ", nullptr, &showStreamingMemory)) {
      Atrum::Debug::SetStreamingMemoryDebugVisible(showStreamingMemory);
    }
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("ビュー")) {
    const SceneViewTab current = EditorFrame::Instance().SceneViewTabSelection();
    if (ImGui::MenuItem(kEditSceneViewTitle, "Ctrl+1",
                         current == SceneViewTab::Edit)) {
      SelectSceneViewTab(SceneViewTab::Edit);
    }
    if (ImGui::MenuItem(kGameSceneViewTitle, "Ctrl+2",
                         current == SceneViewTab::Game)) {
      SelectSceneViewTab(SceneViewTab::Game);
    }
    if (ImGui::MenuItem(kRuntimeSceneViewTitle, "Ctrl+3",
                         current == SceneViewTab::Runtime)) {
      SelectSceneViewTab(SceneViewTab::Runtime);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("ビューを順送り", "`")) {
      CycleSceneViewTab();
    }
    ImGui::TextDisabled("色付きヘッダが現在の表示対象");
    ImGui::EndMenu();
  }

  const SceneViewTab currentTab =
      EditorFrame::Instance().SceneViewTabSelection();
  const bool sceneChanging =
      Atrum::CommandChangeScene::Instance()->GetIsExecute();
  const EditorSceneRunPhase runPhase = QueryEntityEditorSceneRunPhase(
      ui, currentTab, sceneChanging, ui.requestPlayRuntimeScene);

  ImGui::Separator();
  DrawEntityEditorSceneModeToolbar(editor, runPhase);

  const SceneViewTab current = currentTab;
  const char *currentLabel = kEditSceneViewTitle;
  switch (current) {
  case SceneViewTab::Game:
    currentLabel = kGameSceneViewTitle;
    break;
  case SceneViewTab::Runtime:
    currentLabel = kRuntimeSceneViewTitle;
    break;
  default:
    break;
  }
  ImGui::TextDisabled("表示: %s", currentLabel);

  const ImGuiInputFlags shortcutFlags =
      ImGuiInputFlags_Repeat | ImGuiInputFlags_RouteGlobal;
  if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_1, shortcutFlags)) {
    SelectSceneViewTab(SceneViewTab::Edit);
  }
  if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_2, shortcutFlags)) {
    SelectSceneViewTab(SceneViewTab::Game);
  }
  if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_3, shortcutFlags)) {
    SelectSceneViewTab(SceneViewTab::Runtime);
  }
  if (ImGui::Shortcut(ImGuiKey_GraveAccent, shortcutFlags)) {
    CycleSceneViewTab();
  }

  ImGui::TextDisabled("F2: パーティクルエディタ");
  ImGui::EndMainMenuBar();
}

} // namespace

void LayoutEntityEditorWorkbench(EntityEditor::EntityEditor &editor) {
  EditorFrame::Instance().ResetPerFrame();

  editor.ProcessDroppedFileImports();

  DrawMainMenuBar(editor);

  ImGuiViewport *mainViewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(mainViewport->WorkPos);
  ImGui::SetNextWindowSize(mainViewport->WorkSize);
  ImGui::SetNextWindowViewport(mainViewport->ID);
  const ImGuiWindowFlags dockSpaceFlags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("EditorDockSpaceHost", nullptr, dockSpaceFlags);
  ImGui::PopStyleVar(3);

  const ImGuiID dockSpaceId = ImGui::GetID("EntityEditorDockSpaceV4");
  ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f),
                   ImGuiDockNodeFlags_PassthruCentralNode);

  static bool defaultLayoutBuilt = false;
  if (!defaultLayoutBuilt) {
    defaultLayoutBuilt = true;
    ImGui::DockBuilderRemoveNode(dockSpaceId);
    const ImGuiDockNodeFlags dockRootFlags = static_cast<ImGuiDockNodeFlags>(
        static_cast<int>(ImGuiDockNodeFlags_DockSpace) |
        static_cast<int>(ImGuiDockNodeFlags_PassthruCentralNode));
    ImGui::DockBuilderAddNode(dockSpaceId, dockRootFlags);
    ImGui::DockBuilderSetNodeSize(dockSpaceId, mainViewport->WorkSize);

    ImGuiID dockCenter = dockSpaceId;
    const ImGuiID dockLeft =
        ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Left, 0.22f, nullptr,
                                    &dockCenter);
    const ImGuiID dockRight =
        ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.28f, nullptr,
                                    &dockCenter);
    const ImGuiID dockBottom =
        ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, 0.28f, nullptr,
                                    &dockCenter);

    ImGui::DockBuilderDockWindow(kEditSceneViewTitle, dockCenter);
    ImGui::DockBuilderDockWindow(kGameSceneViewTitle, dockCenter);
    ImGui::DockBuilderDockWindow(kRuntimeSceneViewTitle, dockCenter);
    ImGui::DockBuilderDockWindow("ヒエラルキー", dockLeft);
    ImGui::DockBuilderDockWindow("インスペクター", dockRight);
    ImGui::DockBuilderDockWindow("プロジェクト", dockBottom);
    ImGui::DockBuilderDockWindow("タグ", dockBottom);
    ImGui::DockBuilderDockWindow("コードテンプレ", dockBottom);
    ImGui::DockBuilderFinish(dockSpaceId);
  }

  SceneViewPanelOptions sceneViewOptions{};
  sceneViewOptions.showEditView = true;
  sceneViewOptions.showGameView = true;
  sceneViewOptions.showRuntimeView = true;
  sceneViewOptions.showRunPhaseLabel = true;
  sceneViewOptions.runPhase = QueryEntityEditorSceneRunPhase(
      editor.Ui(), EditorFrame::Instance().SceneViewTabSelection(),
      Atrum::CommandChangeScene::Instance()->GetIsExecute(),
      editor.Ui().requestPlayRuntimeScene);
  LayoutEntityEditorSceneViews(sceneViewOptions);

  if (editor.Ui().requestFocusRuntimeSceneView) {
    editor.Ui().requestFocusRuntimeSceneView = false;
    SelectSceneViewTab(SceneViewTab::Runtime);
  }

  if (ImGui::Begin("ヒエラルキー")) {
    editor.DrawHierarchyPanel();
  }
  ImGui::End();

  if (ImGui::Begin("プロジェクト")) {
    editor.DrawProjectPanel();
  }
  ImGui::End();

  if (ImGui::Begin("タグ")) {
    editor.DrawTagsPanel();
  }
  ImGui::End();

  if (ImGui::Begin("インスペクター")) {
    editor.DrawInspectorPanel();
  }
  ImGui::End();

  if (ImGui::Begin("コードテンプレ")) {
    EditorCodeTemplateScaffold::DrawImGuiPanel();
  }
  ImGui::End();

  ImGui::End();
}

} // namespace Atrum::Editor

#endif
