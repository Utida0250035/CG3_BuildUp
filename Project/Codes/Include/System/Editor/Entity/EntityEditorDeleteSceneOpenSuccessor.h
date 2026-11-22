#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Atrum::EntityEditor::EntityEditorDeleteSceneOpenSuccessor {

/// 削除前のソート済み一覧と削除パスから、次に開くべきシーン JSON を決める。
std::optional<std::string>
ResolveAfterDelete(const std::vector<std::string> &sortedBeforeDelete,
                   const std::string &deletedPath);

} // namespace Atrum::EntityEditor::EntityEditorDeleteSceneOpenSuccessor