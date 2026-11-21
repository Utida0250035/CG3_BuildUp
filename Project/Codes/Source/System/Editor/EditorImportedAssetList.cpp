#include "System/Editor/EditorImportedAsset.h"

#include <algorithm>
#include <filesystem>

namespace Atrum::Editor::Import {

namespace fs = std::filesystem;

std::vector<std::string> ListImportedAssetRelativePaths() {
  std::vector<std::string> paths;
  EnsureImportedAssetsDirectoryExists();

  const fs::path root = kImportedAssetsDirectory;
  if (!fs::exists(root) || !fs::is_directory(root)) {
    return paths;
  }

  for (const fs::directory_entry &entry :
       fs::directory_iterator(root, fs::directory_options::skip_permission_denied)) {
    if (!entry.is_regular_file()) {
      continue;
    }
    paths.push_back(entry.path().generic_string());
  }

  std::sort(paths.begin(), paths.end());
  return paths;
}

} // namespace Atrum::Editor::Import