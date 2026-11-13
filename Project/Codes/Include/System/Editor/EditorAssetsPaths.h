#pragma once

namespace Atrum::EditorAssets {

/// エディタ作業用データ（本番シーン・パーティクルは GameAssets）
constexpr const char *kScenesDirectory = "../EditorAssets/Scenes";
constexpr const char *kEntitiesPrefabDirectory = "../EditorAssets/Entities";
constexpr const char *kParticlesDirectory = "../EditorAssets/Particles";

/// ゲーム用 Lua スクリプト（エディタのスクリプト雛形の出力先）
constexpr const char *kScriptsDirectory = "Assets/Scripts";

} // namespace Atrum::EditorAssets
