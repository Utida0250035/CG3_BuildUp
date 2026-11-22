#pragma once

#include "System/Editor/Scene/EditorSceneAsset.h"
#include <string>

namespace Atrum::Editor::LaunchConfigLoadDiagnostics {

enum class LaunchConfigLoadIssue {
  None = 0,
  ConfigFileMissing,
  ConfigOpenFailed,
  ConfigJsonInvalid,
  StartupPathKeyMissing,
  StartupPathEmpty,
  StartupSceneFileMissing,
};

struct LaunchConfigResolveResult {
  std::string startupScenePath{};
  LaunchConfigLoadIssue issue{LaunchConfigLoadIssue::None};
  std::string configPathTried{};
};

LaunchConfigLoadIssue TryLoadLaunchConfigFromPath(
    const std::string &configPath, Scene::LaunchConfig &outConfig);

LaunchConfigResolveResult ResolveStartupScenePathDetailed();

const char *DescribeLaunchConfigLoadIssueJa(LaunchConfigLoadIssue issue);

} // namespace Atrum::Editor::LaunchConfigLoadDiagnostics