#include "System/Editor/Prefab/PrefabAssetFileOperations.h"

#include "System/Editor/Prefab/PrefabAsset.h"

#include <filesystem>

namespace Atrum::Editor::Prefab {

namespace fs = std::filesystem;

namespace {

bool IsSafeNewFileName(const std::string &newFileName) {
  if (newFileName.empty() || newFileName.find('/') != std::string::npos ||
      newFileName.find('\\') != std::string::npos ||
      newFileName.find("..") != std::string::npos) {
    return false;
  }
  return true;
}

bool IsAbsolutePathUnderPrefabRoot(const fs::path &absolutePath) {
  std::error_code ec;
  const fs::path root =
      fs::absolute(kEntitiesPrefabDirectory, ec).lexically_normal();
  if (ec) {
    return false;
  }
  const fs::path normalized = absolutePath.lexically_normal();
  const fs::path relative = fs::relative(normalized, root, ec);
  if (ec || relative.empty()) {
    return false;
  }
  const std::string rel = relative.generic_string();
  return rel.rfind("..", 0) != 0;
}

bool ResolveManagedPrefabFile(const std::string &prefabPath,
                              fs::path &outAbsolute) {
  if (prefabPath.empty()) {
    return false;
  }
  std::error_code ec;
  outAbsolute = fs::absolute(fs::path(prefabPath), ec).lexically_normal();
  if (ec || !fs::exists(outAbsolute) || !fs::is_regular_file(outAbsolute)) {
    return false;
  }
  if (outAbsolute.extension() != ".json") {
    return false;
  }
  return IsAbsolutePathUnderPrefabRoot(outAbsolute);
}

std::string EnsureJsonExtension(std::string fileName) {
  if (fileName.size() < 5 ||
      fileName.compare(fileName.size() - 5, 5, ".json") != 0) {
    fileName += ".json";
  }
  return fileName;
}

} // namespace

bool IsManagedExistingPrefabJsonPath(const std::string &prefabPath) {
  fs::path absolute{};
  return ResolveManagedPrefabFile(prefabPath, absolute);
}

bool TryDeletePrefabJsonFile(const std::string &prefabPath) {
  fs::path absolute{};
  if (!ResolveManagedPrefabFile(prefabPath, absolute)) {
    return false;
  }
  std::error_code ec;
  return fs::remove(absolute, ec) && !ec;
}

std::string TryRenamePrefabJsonFile(const std::string &prefabPath,
                                    const std::string &newFileName) {
  if (!IsSafeNewFileName(newFileName)) {
    return {};
  }

  fs::path absolute{};
  if (!ResolveManagedPrefabFile(prefabPath, absolute)) {
    return {};
  }

  std::error_code ec;
  const fs::path root =
      fs::absolute(kEntitiesPrefabDirectory, ec).lexically_normal();
  if (ec) {
    return {};
  }

  const std::string destFileName = EnsureJsonExtension(newFileName);
  const fs::path dest = (root / destFileName).lexically_normal();
  if (dest.parent_path() != root) {
    return {};
  }

  if (fs::exists(dest) && dest != absolute) {
    return {};
  }

  fs::rename(absolute, dest, ec);
  if (ec) {
    return {};
  }
  return dest.generic_string();
}

} // namespace Atrum::Editor::Prefab