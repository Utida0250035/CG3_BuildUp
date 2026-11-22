#pragma once

namespace Atrum::Editor::EditorEntityWorkbenchLightPostWindow {

/// ImGui ウィンドウ名（`DrawLightingEffectsDebugWindow` と一致）。
const char *WindowTitle();

/// メニューからライト・ポスト調整パネルを開きフォーカスする。
void OpenFromMenu();

} // namespace Atrum::Editor::EditorEntityWorkbenchLightPostWindow