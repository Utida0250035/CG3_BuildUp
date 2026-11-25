#pragma once

namespace Atrum {
class AtrumEngine;
}

namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply {

/// シーンビュー描画直前に方向性ライト定数バッファへカメラ位置を反映する。
void ApplyBeforeDraw(AtrumEngine *engine);

} // namespace Atrum::SceneEntityEditorSceneViewVisualEffectsApply