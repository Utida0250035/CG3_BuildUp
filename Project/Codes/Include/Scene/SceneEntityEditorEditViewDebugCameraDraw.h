#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum {

class SceneEntityEditor;

/// Edit ビュー ImGui::Image ホバー時のみ DebugCamera 入力（Draw 内・GameOrder 外）。
namespace SceneEntityEditorEditViewDebugCameraDraw {

void ApplyHoveredEditViewCameraInput(SceneEntityEditor &editor);

} // namespace SceneEntityEditorEditViewDebugCameraDraw

} // namespace Atrum

#endif