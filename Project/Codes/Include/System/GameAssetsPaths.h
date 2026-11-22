#pragma once

#include "System/Editor/EditorAssetsPaths.h"

namespace Atrum::GameAssets {

/// ランタイムが参照するデータは Project/Assets 以下（開発時 cwd = Project）
constexpr const char *kTagRegistryJson = "./Assets/Tags/tagRegistry.json";

/// Release でも読むシーン JSON
constexpr const char *kScenesDirectory = "./Assets/Scenes";

/// Release でも読むパーティクル JSON（相対パスの基準）
constexpr const char *kParticlesDirectory = "./Assets/Particles";

/// Release 起動時に読むスタートアップシーン指定
constexpr const char *kLaunchConfigPath = "./Assets/launch_config.json";

/// Release 用シーン巡回リスト（Space / パッド A）
constexpr const char *kReleaseSceneCarouselPath =
    "./Assets/release_scene_carousel.json";

/// 本編のオペレータ案内
constexpr const char *kOperatorGuidanceJson =
    "./Assets/UX/operator_guidance.json";

/// 本編 SceneGame のライト／フォグ／ゴッドレイ／影（Release 配布物）
constexpr const char *kGameSceneLightingProfileJson =
    "./Assets/Visual/game_scene_lighting.json";

/// 全方位シューティング atomF の自機・敵パラメータ
constexpr const char *kAtomFParametersJson = "./Assets/atomF/parameters.json";

/// タイトル→本編の間に再生するサンプルカメラトラック
constexpr const char *kTitleIntroMovieTrack = EditorAssets::kSampleMovieTrackJson;

} // namespace Atrum::GameAssets
