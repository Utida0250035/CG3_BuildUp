#pragma once

namespace Atrum {

class SceneBuildUp;

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

/// 機能検証 BuildUp 専用のランタイムビュー ImGui ウィンドウ（JSON パス付きヘッダ）
namespace SceneBuildUpDedicatedRuntimeViewWindow {

constexpr const char *kWindowTitle = "機能検証 BuildUp";

void LayoutAndRenderViews(SceneBuildUp &scene);

} // namespace SceneBuildUpDedicatedRuntimeViewWindow

#endif

} // namespace Atrum