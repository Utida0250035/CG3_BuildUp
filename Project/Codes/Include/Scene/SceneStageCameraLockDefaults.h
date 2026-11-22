#pragma once

namespace Atrum::SceneStageCameraLockDefaults {

/// 新規 Scene 追加時の既定（verify_camera_policy.py と同値）。
/// 制作ツール用シーンのみ IsStageViewCameraLocked() で false を明示する。
inline constexpr bool kDefaultIsStageViewCameraLocked = true;

} // namespace Atrum::SceneStageCameraLockDefaults