#pragma once

namespace Atrum {
class AtrumEngine;
class Camera;
} // namespace Atrum

namespace Atrum::Editor {

/// Unity Scene View 風：世界原点基準 XZ グリッド（適応間隔・10 倍メジャー・X/Z 軸色）
void DrawInfiniteEditorFloorGrid(AtrumEngine *engine, const Atrum::Camera *camera,
                                 int viewportWidth, int viewportHeight);

} // namespace Atrum::Editor
