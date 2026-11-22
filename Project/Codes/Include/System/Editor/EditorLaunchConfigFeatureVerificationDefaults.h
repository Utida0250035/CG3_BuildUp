#pragma once

#include "System/Editor/EditorDebugPlayAreaFeatureVerificationCanonical.h"
#include "System/Editor/EditorAssetsPaths.h"

namespace Atrum::Editor::LaunchConfigFeatureVerificationDefaults {

/// リポジトリ共有 launch_config の既定スタートアップ（機能検証正本）。
inline const char *DefaultStartupScenePath() {
  return DebugPlayAreaFeatureVerificationCanonical::SceneJsonPath();
}

constexpr const char *kEditorLaunchConfigPath = EditorAssets::kEditorLaunchConfigJson;

} // namespace Atrum::Editor::LaunchConfigFeatureVerificationDefaults