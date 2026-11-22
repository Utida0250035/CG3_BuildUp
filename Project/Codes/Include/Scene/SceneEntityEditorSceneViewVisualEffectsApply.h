#pragma once

namespace Atrum {
class Engine;
}

namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply {

/// シーンビュー描画直前にカメラ位置を含め Fog / Rim / GodRays / 影を定数バッファへ反映する。
void ApplyBeforeDraw(Atrum::Engine *engine);

} // namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply