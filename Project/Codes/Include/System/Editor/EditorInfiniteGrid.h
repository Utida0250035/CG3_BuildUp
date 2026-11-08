#pragma once

namespace Atrum {
class AtrumEngine;
class Camera;
} // namespace Atrum

namespace Atrum::Editor {

/// エディタ用 XZ 床グリッド（固定ワールド線幅・メジャー10本ごと・原点 X/Z 軸色）
void DrawInfiniteEditorFloorGrid(AtrumEngine *engine, const Atrum::Camera *camera,
                                 int viewportWidth, int viewportHeight);

} // namespace Atrum::Editor
