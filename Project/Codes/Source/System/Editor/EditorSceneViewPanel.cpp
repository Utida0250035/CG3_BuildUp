#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorSceneViewPanel.h"

#include "System/Editor/EditorSceneRunStatus.h"
#include "System/Editor/EditorSceneRenderTarget.h"
#include "ForDebug/ImGui.h"
#include <imgui/imgui_internal.h>

namespace Atrum::Editor {

namespace {

ImU32 SceneViewTabAccentColor(const SceneViewTab tab, const bool isPrimary) {
  const float alpha = isPrimary ? 0.95f : 0.45f;
  switch (tab) {
  case SceneViewTab::Edit:
    return ImGui::ColorConvertFloat4ToU32(
        ImVec4{0.35f, 0.55f, 0.95f, alpha});
  case SceneViewTab::Game:
    return ImGui::ColorConvertFloat4ToU32(
        ImVec4{0.35f, 0.82f, 0.45f, alpha});
  case SceneViewTab::Runtime:
    return ImGui::ColorConvertFloat4ToU32(
        ImVec4{0.95f, 0.62f, 0.22f, alpha});
  }
  return ImGui::ColorConvertFloat4ToU32(ImVec4{0.6f, 0.6f, 0.6f, alpha});
}

void RegisterSceneViewRenderRequest(EditorFrame &frame, const SceneViewTab tab,
                                    const ImVec2 &displaySize) {
  SceneViewDisplaySize logicalSize{displaySize.x, displaySize.y};
  frame.SetSceneViewDisplaySize(tab, logicalSize);

  const ImVec2 fbScale = ImGui::GetIO().DisplayFramebufferScale;
  SubViewportRect renderSize{};
  renderSize.width = static_cast<int>(displaySize.x * fbScale.x);
  renderSize.height = static_cast<int>(displaySize.y * fbScale.y);
  renderSize.active = renderSize.width > 2 && renderSize.height > 2;
  frame.SetSceneViewRect(tab, renderSize, false);
}

} // namespace

void LayoutDedicatedSceneViewWindow(
    const char *windowTitle, const SceneViewTab tab, const char *statusLine,
    const char *hintText, const bool showRunPhaseLabel,
    const EditorSceneRunPhase runPhase) {
  const ImGuiWindowFlags windowFlags =
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  if (!ImGui::Begin(windowTitle, nullptr, windowFlags)) {
    ImGui::End();
    ImGui::PopStyleVar();
    return;
  }
  ImGui::PopStyleVar();

  ImGuiWindow *window = ImGui::GetCurrentWindow();
  if (window != nullptr && window->DockIsActive && !window->DockTabIsVisible) {
    ImGui::End();
    return;
  }

  EditorFrame &frame = EditorFrame::Instance();
  const bool isPrimary = frame.SceneViewTabSelection() == tab;
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
    frame.SetSceneViewTab(tab);
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
  ImGui::PushStyleColor(ImGuiCol_ChildBg,
                        SceneViewTabAccentColor(tab, isPrimary));
  const float headerHeight = ImGui::GetFrameHeight() + 10.0f;
  if (ImGui::BeginChild("##scene_view_header", ImVec2(0.0f, headerHeight),
                        ImGuiChildFlags_Borders, ImGuiWindowFlags_None)) {
    if (isPrimary) {
      ImGui::TextUnformatted("■ 表示中");
    } else {
      ImGui::TextDisabled("　");
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(statusLine);
    if (hintText != nullptr && hintText[0] != '\0') {
      ImGui::SameLine();
      ImGui::TextDisabled("| %s", hintText);
    }
    if (showRunPhaseLabel) {
      ImGui::SameLine();
      ImGui::TextUnformatted("|");
      ImGui::SameLine();
      DrawEditorSceneRunPhaseBadge(runPhase);
    }
  }
  ImGui::EndChild();
  ImGui::PopStyleColor();
  ImGui::PopStyleVar();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  const ImGuiWindowFlags childFlags =
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
  const float remainingY = ImGui::GetContentRegionAvail().y;
  if (ImGui::BeginChild("##scene_view_render", ImVec2(0.0f, remainingY),
                        ImGuiChildFlags_Borders, childFlags)) {
    const ImVec2 imageSize = ImGui::GetContentRegionAvail();
    RegisterSceneViewRenderRequest(frame, tab, imageSize);

    const ImTextureRef texture =
        EditorSceneRenderTargetHub::Instance().ImGuiTexture(tab);
    if (texture.GetTexID() != ImTextureID_Invalid && imageSize.x > 2.0f &&
        imageSize.y > 2.0f) {
      ImGui::Image(texture, imageSize);
      const bool hovered = ImGui::IsItemHovered();
      SubViewportRect renderRect = frame.SceneViewRect(tab);
      frame.SetSceneViewRect(tab, renderRect, hovered);
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        frame.SetSceneViewTab(tab);
      }
    } else if (imageSize.x > 2.0f && imageSize.y > 2.0f) {
      ImGui::Dummy(imageSize);
      ImGui::SetCursorPos(ImGui::GetItemRectMin());
      ImGui::TextDisabled("シーン描画を準備中...");
    }
  }
  ImGui::EndChild();
  ImGui::PopStyleVar();

  ImGui::End();
}

void LayoutEntityEditorSceneViews(const SceneViewPanelOptions &options) {
  const bool showPhase = options.showRunPhaseLabel;
  const EditorSceneRunPhase phase = options.runPhase;
  if (options.showEditView) {
    LayoutDedicatedSceneViewWindow(
        kEditSceneViewTitle, SceneViewTab::Edit,
        "エディットビュー — デバッグカメラ・Unity風グリッド", nullptr,
        showPhase, phase);
  }
  if (options.showGameView) {
    LayoutDedicatedSceneViewWindow(
        kGameSceneViewTitle, SceneViewTab::Game,
        "ゲームビュー — 初期状態（ステージカメラ・停止）", nullptr, showPhase,
        phase);
  }
  if (options.showRuntimeView) {
    LayoutDedicatedSceneViewWindow(
        kRuntimeSceneViewTitle, SceneViewTab::Runtime,
        "実行時ビュー — シミュレーション実行", nullptr, showPhase, phase);
  }
}

} // namespace Atrum::Editor

#endif
