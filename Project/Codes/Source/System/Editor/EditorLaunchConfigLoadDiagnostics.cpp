#include "System/Editor/EditorLaunchConfigLoadDiagnostics.h"

#include "System/Editor/Scene/EditorSceneAsset.h"
#include "System/Editor/EditorLaunchConfigJsonDocument.h"
#include "System/Editor/EditorLaunchConfigSchema.h"
#include "System/Editor/EditorAssetsPaths.h"
#include "System/GameAssetsPaths.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace Atrum::Editor::LaunchConfigLoadDiagnostics {

namespace fs = std::filesystem;

LaunchConfigLoadIssue TryLoadLaunchConfigFromPath(
    const std::string &configPath,
    ::Atrum::Editor::Scene::LaunchConfig &outConfig) {
  outConfig = {};
  if (configPath.empty()) {
    return LaunchConfigLoadIssue::ConfigFileMissing;
  }
  if (!fs::exists(configPath)) {
    return LaunchConfigLoadIssue::ConfigFileMissing;
  }

  std::ifstream file(configPath);
  if (!file.is_open()) {
    return LaunchConfigLoadIssue::ConfigOpenFailed;
  }

  nlohmann::json json{};
  try {
    file >> json;
  } catch (...) {
    return LaunchConfigLoadIssue::ConfigJsonInvalid;
  }

  if (json.contains(LaunchConfigSchema::kSchemaVersionKey) &&
      json.at(LaunchConfigSchema::kSchemaVersionKey).is_number_integer()) {
    outConfig.schemaVersion =
        json.at(LaunchConfigSchema::kSchemaVersionKey).get<int>();
  }

  if (!LaunchConfigJsonDocument::TryParseStartupScenePath(json,
                                                          outConfig.startupScenePath)) {
    return LaunchConfigLoadIssue::StartupPathKeyMissing;
  }
  if (outConfig.startupScenePath.empty()) {
    return LaunchConfigLoadIssue::StartupPathEmpty;
  }
  if (!fs::exists(outConfig.startupScenePath)) {
    return LaunchConfigLoadIssue::StartupSceneFileMissing;
  }
  return LaunchConfigLoadIssue::None;
}

LaunchConfigResolveResult ResolveStartupScenePathDetailed() {
  LaunchConfigResolveResult result{};
  Scene::LaunchConfig config{};

  result.configPathTried = GameAssets::kLaunchConfigPath;
  const LaunchConfigLoadIssue primaryIssue =
      TryLoadLaunchConfigFromPath(result.configPathTried, config);
  if (primaryIssue == LaunchConfigLoadIssue::None) {
    result.startupScenePath = config.startupScenePath;
    result.issue = primaryIssue;
    return result;
  }

  config = {};
  result.configPathTried = EditorAssets::kEditorLaunchConfigJson;
  const LaunchConfigLoadIssue editorIssue =
      TryLoadLaunchConfigFromPath(result.configPathTried, config);
  if (editorIssue == LaunchConfigLoadIssue::None) {
    result.startupScenePath = config.startupScenePath;
    result.issue = editorIssue;
    return result;
  }

  result.issue = primaryIssue != LaunchConfigLoadIssue::ConfigFileMissing
                     ? primaryIssue
                     : editorIssue;
  if (result.configPathTried.empty()) {
    result.configPathTried = GameAssets::kLaunchConfigPath;
  }
  return result;
}

const char *DescribeLaunchConfigLoadIssueJa(LaunchConfigLoadIssue issue) {
  switch (issue) {
  case LaunchConfigLoadIssue::None:
    return "起動シーン解決済み";
  case LaunchConfigLoadIssue::ConfigFileMissing:
    return "launch_config が見つかりません";
  case LaunchConfigLoadIssue::ConfigOpenFailed:
    return "launch_config を開けません";
  case LaunchConfigLoadIssue::ConfigJsonInvalid:
    return "launch_config の JSON が不正です";
  case LaunchConfigLoadIssue::StartupPathKeyMissing:
    return "startupScenePath キーがありません";
  case LaunchConfigLoadIssue::StartupPathEmpty:
    return "startupScenePath が空です（launch_config は存在）";
  case LaunchConfigLoadIssue::StartupSceneFileMissing:
    return "startupScenePath のシーン JSON が見つかりません";
  }
  return "launch_config の読込に失敗しました";
}

} // namespace Atrum::Editor::LaunchConfigLoadDiagnostics