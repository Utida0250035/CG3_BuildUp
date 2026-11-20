#pragma once

namespace Atrum::Editor::LaunchConfigSchema {

/// launch_config.json の現在スキーマ（拡張時にインクリメント）
constexpr int kVersion = 1;

constexpr const char *kSchemaVersionKey = "schemaVersion";
constexpr const char *kStartupScenePathKey = "startupScenePath";
constexpr const char *kDefaultFeatureVerificationScenePathKey =
    "defaultFeatureVerificationScenePath";

} // namespace Atrum::Editor::LaunchConfigSchema