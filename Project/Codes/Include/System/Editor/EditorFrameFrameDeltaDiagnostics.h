#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::Editor::EditorFrameFrameDeltaDiagnostics {

/// メインメニュー「デバッグ」からフレーム時間 UI を切替
void DrawMenuItems();

} // namespace Atrum::Editor::EditorFrameFrameDeltaDiagnostics

#endif