#pragma once

namespace Atrum::SceneBuildUpAllowDebugCameraContract {

/// SceneBuildUp は AllowDebugCameraControl を override しない。
/// プレイヤ向けデバッグカメラ入力は Scene 既定（false）と
/// SceneDebugCameraInputPolicy（ロック時も false）に委ねる。
constexpr bool kInheritsSceneAllowDebugCameraControl = true;

} // namespace Atrum::SceneBuildUpAllowDebugCameraContract