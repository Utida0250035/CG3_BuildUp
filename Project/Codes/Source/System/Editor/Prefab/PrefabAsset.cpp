#include "System/Editor/Prefab/PrefabAsset.h"

#include <algorithm>
#include <filesystem>

namespace Atrum::Editor::Prefab {

std::vector<std::string> ListPrefabJsonFiles(
    const std::string &directoryRelativePath) {
  namespace fs = std::filesystem;
  std::vector<std::string> paths;
  const fs::path root = directoryRelativePath;
  if (!fs::exists(root) || !fs::is_directory(root)) {
    return paths;
  }
  for (const fs::directory_entry &entry : fs::directory_iterator(root)) {
    if (!entry.is_regular_file()) {
      continue;
    }
    if (entry.path().extension() != ".json") {
      continue;
    }
    paths.push_back(entry.path().generic_string());
  }
  std::sort(paths.begin(), paths.end());
  return paths;
}

} // namespace Atrum::Editor::Prefab
