#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Editor::EditorFrameLightingDiagnostics {

/// 「デバッグ」メニュー内に挿入する項目（Fog／GodRays／影）。
void DrawMenuItems();

/// BuildUp ランタイムビュー等向けの最小メニューバー。
void DrawMainMenuBar();

} // namespace Atrum::Editor::EditorFrameLightingDiagnostics

#endif