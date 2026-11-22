#include "System/Editor/EditorImportedAsset.h"

#include <filesystem>

namespace Atrum::Editor::Import {

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

bool IsAbsolutePathUnderImportedRoot(const fs::path &absolutePath) {
  std::error_code ec;
  const fs::path root =
      fs::absolute(kImportedAssetsDirectory, ec).lexically_normal();
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

bool ResolveManagedImportedFile(const std::string &relativePath,
                                fs::path &outAbsolute) {
  if (relativePath.empty()) {
    return false;
  }
  std::error_code ec;
  outAbsolute = fs::absolute(fs::path(relativePath), ec).lexically_normal();
  if (ec || !fs::exists(outAbsolute) || !fs::is_regular_file(outAbsolute)) {
    return false;
  }
  return IsAbsolutePathUnderImportedRoot(outAbsolute);
}

} // namespace

bool TryDeleteImportedAssetFile(const std::string &relativePath) {
  fs::path absolute{};
  if (!ResolveManagedImportedFile(relativePath, absolute)) {
    return false;
  }
  std::error_code ec;
  return fs::remove(absolute, ec) && !ec;
}

std::string TryRenameImportedAssetFile(const std::string &relativePath,
                                       const std::string &newFileName) {
  if (!IsSafeNewFileName(newFileName)) {
    return {};
  }

  fs::path absolute{};
  if (!ResolveManagedImportedFile(relativePath, absolute)) {
    return {};
  }

  std::error_code ec;
  const fs::path root =
      fs::absolute(kImportedAssetsDirectory, ec).lexically_normal();
  if (ec) {
    return {};
  }

  const fs::path dest = (root / newFileName).lexically_normal();
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

} // namespace Atrum::Editor::Import