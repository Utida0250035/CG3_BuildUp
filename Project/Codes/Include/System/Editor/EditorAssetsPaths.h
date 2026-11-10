#pragma once

namespace Atrum::EditorAssets {

/// エディタ専用データ（ゲーム本番の Update/Draw では読まない）
constexpr const char *kScenesDirectory = "../EditorAssets/Scenes";
constexpr const char *kEntitiesPrefabDirectory = "../EditorAssets/Entities";
constexpr const char *kParticlesDirectory = "../EditorAssets/Particles";

/// Release 起動時に読むスタートアップシーン指定（リポジトリにコミット）
constexpr const char *kLaunchConfigPath = "../EditorAssets/launch_config.json";

} // namespace Atrum::EditorAssets
