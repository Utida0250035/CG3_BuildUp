#pragma once

#include <string>

namespace Atrum::Editor::LaunchConfigStartupPathNormalize {

/// launch_config に保存する startupScenePath を portable な相対／正規形式へ揃える。
std::string NormalizeForLaunchConfig(const std::string &sceneJsonPath);

} // namespace Atrum::Editor::LaunchConfigStartupPathNormalize