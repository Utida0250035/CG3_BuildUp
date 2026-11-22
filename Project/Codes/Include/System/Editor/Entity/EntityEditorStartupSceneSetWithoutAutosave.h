#pragma once

#include <string>

namespace Atrum::EntityEditor {
class EntityEditor;
}

namespace Atrum::EntityEditor::EntityEditorStartupSceneSetWithoutAutosave {

std::string ResolveStartupTargetPath(const EntityEditor &editor);

/// launch_config のみ更新（シーン JSON は自動保存しない）。
bool SetOnly(EntityEditor &editor, std::string *statusMessage);

/// 起動シーン設定後 BuildUp を予約。編集中パス一致時のみ requestSaveToDisk をキュー。
bool SetAndQueueBuildUp(EntityEditor &editor, std::string *statusMessage);

} // namespace Atrum::EntityEditor::EntityEditorStartupSceneSetWithoutAutosave