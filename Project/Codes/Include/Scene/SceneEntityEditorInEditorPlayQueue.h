#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::EntityEditor {

struct EntityEditorUi;

/// エディタ内プレビュー（Runtime タブ切替のみ。SceneBuildUp は起動しない）。
void QueueInEditorPlayRuntimeTabOnly(EntityEditorUi &ui);

} // namespace Atrum::EntityEditor

#endif