#include "System/Editor/EditorEntityWorkbenchDefaultDockLayout.h"

#include "System/Editor/EditorEntityWorkbenchLightPostWindow.h"
#include "System/Editor/EditorSceneViewPanel.h"

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include <imgui/imgui_internal.h>
#endif

namespace Atrum::Editor::EditorEntityWorkbenchDefaultDockLayout {

namespace {

int &AppliedLayoutVersion() {
  static int version = 0;
  return version;
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

void BuildDefaultLayout(const ImGuiID dockSpaceId, const ImVec2 workSize) {
  ImGui::DockBuilderRemoveNode(dockSpaceId);
  const ImGuiDockNodeFlags dockRootFlags = static_cast<ImGuiDockNodeFlags>(
      static_cast<int>(ImGuiDockNodeFlags_DockSpace) |
      static_cast<int>(ImGuiDockNodeFlags_PassthruCentralNode));
  ImGui::DockBuilderAddNode(dockSpaceId, dockRootFlags);
  ImGui::DockBuilderSetNodeSize(dockSpaceId, workSize);

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
  ImGui::DockBuilderDockWindow("スクリプト", dockBottom);
  ImGui::DockBuilderDockWindow("Stage camera tuning", dockBottom);
  ImGui::DockBuilderDockWindow(
      EditorEntityWorkbenchLightPostWindow::WindowTitle(), dockBottom);
  ImGui::DockBuilderFinish(dockSpaceId);
}

#endif

} // namespace

const char *DockSpaceIdLabel() { return "EntityEditorDockSpaceV5"; }

void ApplyIfNeeded(const ImGuiID dockSpaceId, const ImVec2 workSize,
                   const bool forceRebuild) {
#if defined(DEVELOPMENT) && defined(USE_IMGUI)
  if (!forceRebuild &&
      AppliedLayoutVersion() >= kDefaultDockLayoutVersion) {
    return;
  }
  AppliedLayoutVersion() = kDefaultDockLayoutVersion;
  BuildDefaultLayout(dockSpaceId, workSize);
#else
  (void)dockSpaceId;
  (void)workSize;
  (void)forceRebuild;
#endif
}

} // namespace Atrum::Editor::EditorEntityWorkbenchDefaultDockLayout