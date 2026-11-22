#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::EntityEditor {

struct UiState;

/// 「エディタ内プレビュー」後は Runtime タブ表示とウィンドウフォーカスのみ（自動実行モードは付けない）。
void ApplyInEditorPlayRuntimeTabFocusOnly(UiState &ui);

} // namespace Atrum::EntityEditor

#endif