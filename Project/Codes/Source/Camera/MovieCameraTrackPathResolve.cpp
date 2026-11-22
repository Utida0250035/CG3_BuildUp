#include "Camera/MovieCameraTrackPathResolve.h"

#include "System/Editor/EditorAssetsPaths.h"

#include <filesystem>

namespace Atrum {

namespace {

std::string TryExistingPath(const std::string &candidate) {
  if (candidate.empty()) {
    return {};
  }
  namespace fs = std::filesystem;
  std::error_code ec{};
  if (fs::exists(candidate, ec) && !ec) {
    return candidate;
  }
  return {};
}

} // namespace

std::string ResolveMovieCameraTrackJsonPath(const std::string &path) {
  if (path.empty()) {
    return {};
  }

  if (const std::string direct = TryExistingPath(path); !direct.empty()) {
    return direct;
  }

  const std::string underCameraDir =
      std::string(EditorAssets::kMovieCameraTracksDirectory) + "/" + path;
  if (const std::string resolved = TryExistingPath(underCameraDir);
      !resolved.empty()) {
    return resolved;
  }

  const std::string fileName = std::filesystem::path(path).filename().string();
  if (fileName != path) {
    const std::string byFileName =
        std::string(EditorAssets::kMovieCameraTracksDirectory) + "/" + fileName;
    if (const std::string resolved = TryExistingPath(byFileName);
        !resolved.empty()) {
      return resolved;
    }
  }

  if (path.rfind("EditorAssets/", 0) == 0 || path.rfind("EditorAssets\\", 0) == 0) {
    const std::string fromRepoRoot = "../" + path;
    if (const std::string resolved = TryExistingPath(fromRepoRoot);
        !resolved.empty()) {
      return resolved;
    }
  }

  if (path.rfind("Camera/", 0) == 0 || path.rfind("Camera\\", 0) == 0) {
    const std::string underEditorAssets = std::string("../EditorAssets/") + path;
    if (const std::string resolved = TryExistingPath(underEditorAssets);
        !resolved.empty()) {
      return resolved;
    }
  }

  return path;
}

} // namespace Atrum