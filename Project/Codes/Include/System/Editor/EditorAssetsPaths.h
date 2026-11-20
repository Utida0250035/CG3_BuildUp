#pragma once

namespace Atrum::EditorAssets {

/// エディタ作業用データ（本番シーン・パーティクルは GameAssets）
constexpr const char *kScenesDirectory = "../EditorAssets/Scenes";
constexpr const char *kEntitiesPrefabDirectory = "../EditorAssets/Entities";
constexpr const char *kParticlesDirectory = "../EditorAssets/Particles";
constexpr const char *kStageViewCameraTuningJson =
    "../EditorAssets/Camera/stage_view_camera_tuning.json";
constexpr const char *kGameSceneLightingProfileJson =
    "../EditorAssets/Visual/game_scene_lighting.json";
constexpr const char *kMovieCameraTracksDirectory = "../EditorAssets/Camera";
constexpr const char *kSampleMovieTrackJson =
    "../EditorAssets/Camera/sample_movie_track.json";
constexpr const char *kDebugPlayAreaDirectory = "../EditorAssets/DebugPlayArea";
constexpr const char *kDebugPlayAreaTemplateScene =
    "../EditorAssets/DebugPlayArea/template_test_play_area.scene.json";
/// 機能検証 BuildUp 用の同梱正本（scaffold テンプレとは別）
constexpr const char *kDebugPlayAreaLoopTick13DemoScene =
    "../EditorAssets/DebugPlayArea/LoopTick13Demo.scene.json";
constexpr const char *kDebugPlayAreaCombatBoundsSandboxScene =
    "../EditorAssets/DebugPlayArea/combat_bounds_sandbox.scene.json";

/// ゲーム用 Lua スクリプト（エディタのスクリプト雛形の出力先）
constexpr const char *kScriptsDirectory = "Assets/Scripts";

} // namespace Atrum::EditorAssets
