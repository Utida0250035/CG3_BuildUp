#pragma once

namespace Atrum::Editor::EditorFrameFitCameraPerspectiveParams {

/// stage_view_camera_tuning.json の perspectiveFovY（未読込時は Ensure 後の既定）。
float ResolveViewportPerspectiveFovY();

constexpr float kViewportNearClip = 0.125f;
constexpr float kViewportFarClip = 128.0f;

} // namespace Atrum::Editor::EditorFrameFitCameraPerspectiveParams