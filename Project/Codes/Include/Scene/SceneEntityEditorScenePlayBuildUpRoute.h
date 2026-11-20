#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum::EntityEditor {

struct EntityEditorUi;

/// プロジェクトパネル「シーンを実行」を本番同等の SceneBuildUp フェード遷移へ載せる。
void QueueEditorScenePlayViaBuildUp(EntityEditorUi &ui);

} // namespace Atrum::EntityEditor

#endif