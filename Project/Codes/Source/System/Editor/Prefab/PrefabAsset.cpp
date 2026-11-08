#include "System/Editor/Prefab/PrefabAsset.h"

#include <algorithm>
#include <filesystem>

namespace Atrum::Editor::Prefab {

void EnsureEntitiesPrefabDirectoryExists() {
  namespace fs = std::filesystem;
  const fs::path root = kEntitiesPrefabDirectory;
  if (!fs::exists(root)) {
    fs::create_directories(root);
  }
}

std::string MakeUniquePrefabFilePath(const std::string &baseName) {
  namespace fs = std::filesystem;
  EnsureEntitiesPrefabDirectoryExists();

  std::string stem = baseName.empty() ? "Entity" : baseName;
  for (char &ch : stem) {
    if (ch == '/' || ch == '\\' || ch == ':' || ch == '"' ||
        ch == '*' || ch == '?' || ch == '<' || ch == '>' || ch == '|') {
      ch = '_';
    }
  }

  fs::path candidate =
      fs::path(kEntitiesPrefabDirectory) / (stem + ".json");
  if (!fs::exists(candidate)) {
    return candidate.generic_string();
  }

  for (int suffix = 1; suffix < 10000; ++suffix) {
    candidate = fs::path(kEntitiesPrefabDirectory) /
                (stem + "_" + std::to_string(suffix) + ".json");
    if (!fs::exists(candidate)) {
      return candidate.generic_string();
    }
  }

  return {};
}

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
