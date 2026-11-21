#pragma once

#include <string>

namespace Atrum::EntityEditor {
struct UiState;
}

namespace Atrum::Editor::EditorImportedAssetUiPathScrub {

/// Imported ファイル削除・改名後に UI が保持するパス参照を更新する。
void OnImportedAssetPathRemoved(EntityEditor::UiState &ui,
                                const std::string &removedPath);

void OnImportedAssetPathRenamed(EntityEditor::UiState &ui,
                                const std::string &oldPath,
                                const std::string &newPath);

} // namespace Atrum::Editor::EditorImportedAssetUiPathScrub