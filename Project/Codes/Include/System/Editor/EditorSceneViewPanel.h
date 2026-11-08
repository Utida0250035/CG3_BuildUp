#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrame.h"

namespace Atrum::Editor {

constexpr const char *kEditSceneViewTitle = "エディットビュー";
constexpr const char *kGameSceneViewTitle = "ゲームビュー";
constexpr const char *kRuntimeSceneViewTitle = "実行時ビュー";

struct SceneViewPanelOptions {
  bool showEditView = true;
  bool showGameView = true;
  bool showRuntimeView = true;
  const char *hintText = nullptr;
};

/// 種別ごとのシーンビュー ImGui ウィンドウ（描画領域は背景透明）
void LayoutDedicatedSceneViewWindow(const char *windowTitle, SceneViewTab tab,
                                    const char *statusLine,
                                    const char *hintText = nullptr);

void LayoutEntityEditorSceneViews(const SceneViewPanelOptions &options);

} // namespace Atrum::Editor

#endif
