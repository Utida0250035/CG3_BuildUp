#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::EntityEditor {

struct UiState;

/// エディタ内プレビュー（Runtime タブ切替のみ。SceneBuildUp は起動しない）。
void QueueInEditorPlayRuntimeTabOnly(UiState &ui);

} // namespace Atrum::EntityEditor

#endif