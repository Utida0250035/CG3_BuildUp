#pragma once

namespace Atrum {
class AtrumEngine;
class Camera;
} // namespace Atrum

namespace Atrum::AtomF {

/// 自機の真上に、最大HP個のハートを中央揃えで描く（残りHP分が点灯、失った分は暗色）。
/// viewWidth/viewHeight は描画先ビューのピクセルサイズ。
void DrawPlayerHealthIconsAbovePlayer(AtrumEngine *engine, const Camera &camera,
                                      float viewWidth, float viewHeight);

} // namespace Atrum::AtomF
