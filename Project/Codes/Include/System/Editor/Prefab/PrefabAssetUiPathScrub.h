#pragma once

#include <string>

namespace Atrum::Editor::Entity {
struct UiState;
}

namespace Atrum::Editor::Prefab::PrefabAssetUiPathScrub {

void OnPrefabPathRemoved(EntityEditor::UiState &ui,
                         const std::string &removedPath);

void OnPrefabPathRenamed(EntityEditor::UiState &ui,
                         const std::string &oldPath,
                         const std::string &newPath);

} // namespace Atrum::Editor::Prefab::PrefabAssetUiPathScrub