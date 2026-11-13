#include "System/Editor/Scene/EditorSceneAsset.h"

#include "System/GameAssetsPaths.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <system_error>

namespace Atrum::Editor::Scene {

void EnsureScenesDirectoryExists() {
  namespace fs = std::filesystem;
  const fs::path root = kScenesDirectory;
  if (!fs::exists(root)) {
    fs::create_directories(root);
  }
}

std::vector<std::string> ListSceneJsonFiles(
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

std::vector<std::string> ListEditableSceneJsonFiles() {
  std::vector<std::string> paths =
      ListSceneJsonFiles(GameAssets::kScenesDirectory);
  const std::vector<std::string> editorPaths = ListSceneJsonFiles();
  paths.insert(paths.end(), editorPaths.begin(), editorPaths.end());
  return paths;
}

std::string MakeUniqueSceneFilePath() {
  namespace fs = std::filesystem;
  EnsureScenesDirectoryExists();
  for (int index = 0; index < 100000; ++index) {
    const std::string path =
        std::string(kScenesDirectory) + "/Scene_" + std::to_string(index) +
        ".json";
    if (!fs::exists(path)) {
      return path;
    }
  }
  return std::string(kScenesDirectory) + "/Scene_overflow.json";
}

std::string SceneDisplayNameFromPath(const std::string &jsonFilePath) {
  namespace fs = std::filesystem;
  return fs::path(jsonFilePath).stem().string();
}

bool TryDeleteSceneJsonFile(const std::string &jsonFilePath) {
  namespace fs = std::filesystem;
  if (jsonFilePath.empty()) {
    return false;
  }

  const fs::path target = fs::path(jsonFilePath).lexically_normal();
  if (target.extension() != ".json") {
    return false;
  }

  std::error_code ec{};
  const fs::path absoluteTarget = fs::absolute(target, ec).lexically_normal();
  if (ec) {
    return false;
  }

  const char *roots[] = {GameAssets::kScenesDirectory, kScenesDirectory};
  bool insideManagedRoot = false;
  for (const char *root : roots) {
    const fs::path scenesRoot = fs::absolute(root, ec).lexically_normal();
    if (ec) {
      ec.clear();
      continue;
    }
    const fs::path relative = fs::relative(absoluteTarget, scenesRoot, ec);
    if (ec || relative.empty()) {
      ec.clear();
      continue;
    }
    const std::string relativeGeneric = relative.generic_string();
    if (relativeGeneric.rfind("..", 0) == 0 ||
        relativeGeneric.find("/../") != std::string::npos) {
      continue;
    }
    insideManagedRoot = true;
    break;
  }
  if (!insideManagedRoot) {
    return false;
  }

  if (!fs::is_regular_file(absoluteTarget, ec) || ec) {
    return false;
  }

  return fs::remove(absoluteTarget, ec) && !ec;
}

bool TryLoadLaunchConfig(LaunchConfig &outConfig) {
  namespace fs = std::filesystem;
  outConfig = {};
  const fs::path configPath = GameAssets::kLaunchConfigPath;
  if (!fs::exists(configPath)) {
    return false;
  }

  std::ifstream file(configPath);
  if (!file.is_open()) {
    return false;
  }

  nlohmann::json json{};
  try {
    file >> json;
  } catch (...) {
    return false;
  }

  if (json.contains("startupScenePath") &&
      json.at("startupScenePath").is_string()) {
    outConfig.startupScenePath = json.at("startupScenePath").get<std::string>();
  }
  return !outConfig.startupScenePath.empty();
}

bool TrySaveLaunchConfig(const LaunchConfig &config) {
  if (config.startupScenePath.empty()) {
    return false;
  }

  namespace fs = std::filesystem;
  const fs::path configPath = GameAssets::kLaunchConfigPath;
  std::error_code ec{};
  fs::create_directories(configPath.parent_path(), ec);

  nlohmann::json json{};
  json["startupScenePath"] = config.startupScenePath;

  std::ofstream file(configPath);
  if (!file.is_open()) {
    return false;
  }
  file << json.dump(4);
  file.flush();
  return file.good();
}

bool TrySetRepositoryStartupScene(const std::string &sceneJsonPath) {
  if (sceneJsonPath.empty()) {
    return false;
  }
  namespace fs = std::filesystem;
  if (!fs::exists(sceneJsonPath) || fs::path(sceneJsonPath).extension() != ".json") {
    return false;
  }
  LaunchConfig config{};
  config.startupScenePath = sceneJsonPath;
  if (!TrySaveLaunchConfig(config)) {
    return false;
  }
  EditorScenePlayRequest::Instance().Set(sceneJsonPath);
  return true;
}

std::string ResolveStartupScenePath() {
  LaunchConfig config{};
  if (!TryLoadLaunchConfig(config)) {
    return {};
  }
  namespace fs = std::filesystem;
  if (!fs::exists(config.startupScenePath)) {
    return {};
  }
  return config.startupScenePath;
}

EditorScenePlayRequest &EditorScenePlayRequest::Instance() {
  static EditorScenePlayRequest instance;
  return instance;
}

void EditorScenePlayRequest::Set(const std::string &jsonFilePath) {
  jsonFilePath_ = jsonFilePath;
}

void EditorScenePlayRequest::Clear() { jsonFilePath_.clear(); }

} // namespace Atrum::Editor::Scene
