#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Editor::EditorFrameStreamingDiagnostics {

/// 「デバッグ」メニュー内のストリーミング診断トグル（EntityEditor／BuildUp 共通）。
void DrawMenuItems();

} // namespace Atrum::Editor::EditorFrameStreamingDiagnostics

#endif