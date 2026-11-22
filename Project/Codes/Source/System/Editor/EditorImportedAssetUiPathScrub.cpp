#include "System/Editor/EditorImportedAssetUiPathScrub.h"

#include "System/Editor/Entity/EntityEditor.h"

#include <algorithm>

namespace Atrum::Editor::EditorImportedAssetUiPathScrub {

void OnImportedAssetPathRemoved(EntityEditor::UiState &ui,
                                const std::string &removedPath) {
  if (ui.lastImportedAssetPath == removedPath) {
    ui.lastImportedAssetPath.clear();
  }
  ui.recentImportedAssetPaths.erase(
      std::remove(ui.recentImportedAssetPaths.begin(),
                  ui.recentImportedAssetPaths.end(), removedPath),
      ui.recentImportedAssetPaths.end());
}

void OnImportedAssetPathRenamed(EntityEditor::UiState &ui,
                                const std::string &oldPath,
                                const std::string &newPath) {
  if (ui.lastImportedAssetPath == oldPath) {
    ui.lastImportedAssetPath = newPath;
  }
  for (std::string &path : ui.recentImportedAssetPaths) {
    if (path == oldPath) {
      path = newPath;
    }
  }
}

} // namespace Atrum::Editor::EditorImportedAssetUiPathScrub