#pragma once

namespace Atrum {
class AtrumEngine;
}

namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply {

/// シーンビュー描画直前にカメラ位置を含め Fog / Rim / GodRays / 影を定数バッファへ反映する。
void ApplyBeforeDraw(AtrumEngine *engine);

} // namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply