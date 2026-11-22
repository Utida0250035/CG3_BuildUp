#include "Scene/SceneBuildUpSceneJsonPathResolve.h"

#include "System/Editor/EditorAssetsPaths.h"
#include "System/GameAssetsPaths.h"

#include <filesystem>
#include <string>
#include <vector>

namespace Atrum {

namespace {

bool PathExists(const std::string &candidate) {
  if (candidate.empty()) {
    return false;
  }
  namespace fs = std::filesystem;
  std::error_code error{};
  return fs::exists(candidate, error) && !error;
}

std::string CanonicalIfExists(const std::string &candidate) {
  if (!PathExists(candidate)) {
    return {};
  }
  namespace fs = std::filesystem;
  try {
    return fs::weakly_canonical(fs::path(candidate)).generic_string();
  } catch (...) {
    return candidate;
  }
}

void PushUnique(std::vector<std::string> &candidates,
                const std::string &candidate) {
  if (candidate.empty()) {
    return;
  }
  for (const std::string &existing : candidates) {
    if (existing == candidate) {
      return;
    }
  }
  candidates.push_back(candidate);
}

std::string NormalizeSlashes(std::string path) {
  for (char &ch : path) {
    if (ch == '\\') {
      ch = '/';
    }
  }
  return path;
}

} // namespace

std::string ResolveSceneBuildUpJsonPath(const std::string &path) {
  if (path.empty()) {
    return {};
  }

  std::vector<std::string> candidates{};
  PushUnique(candidates, path);

  const std::filesystem::path inputPath(path);
  const std::string fileName = inputPath.filename().string();

  if (path.rfind("./", 0) != 0 && path.rfind("../", 0) != 0 &&
      path.front() != '/') {
    PushUnique(candidates,
               std::string(GameAssets::kScenesDirectory) + "/" + path);
    PushUnique(candidates,
               std::string(EditorAssets::kScenesDirectory) + "/" + path);
    PushUnique(candidates,
               std::string(EditorAssets::kDebugPlayAreaDirectory) + "/" + path);
  }

  if (!fileName.empty() && fileName != path) {
    PushUnique(candidates,
               std::string(GameAssets::kScenesDirectory) + "/" + fileName);
    PushUnique(candidates,
               std::string(EditorAssets::kScenesDirectory) + "/" + fileName);
    PushUnique(candidates, std::string(EditorAssets::kDebugPlayAreaDirectory) +
                              "/" + fileName);
  }

  const std::string normalized = NormalizeSlashes(path);
  if (normalized.rfind("EditorAssets/", 0) == 0) {
    PushUnique(candidates, "../" + normalized);
  }
  if (normalized.rfind("Assets/Scenes/", 0) == 0) {
    PushUnique(candidates, "./" + normalized);
  }

  for (const std::string &candidate : candidates) {
    if (const std::string resolved = CanonicalIfExists(candidate);
        !resolved.empty()) {
      return resolved;
    }
  }

  return path;
}

} // namespace Atrum