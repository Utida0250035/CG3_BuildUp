#include "System/Editor/Prefab/PrefabAssetUiPathScrub.h"

#include "System/Editor/Entity/EntityEditor.h"

namespace Atrum::Editor::Prefab::PrefabAssetUiPathScrub {

void OnPrefabPathRemoved(EntityEditor::UiState &ui,
                         const std::string &removedPath) {
  if (ui.selectedPrefabPath == removedPath) {
    ui.selectedPrefabPath.clear();
  }
}

void OnPrefabPathRenamed(EntityEditor::UiState &ui,
                         const std::string &oldPath,
                         const std::string &newPath) {
  if (ui.selectedPrefabPath == oldPath) {
    ui.selectedPrefabPath = newPath;
  }
}

} // namespace Atrum::Editor::Prefab::PrefabAssetUiPathScrub