#pragma once

namespace Atrum {
class AtrumEngine;
} // namespace Atrum

namespace Atrum::AtomF {

/// 画面下部中央にボスの Hp バーを描く（ボスが居ない間は何もしない）。
/// viewWidth/viewHeight は描画先ビューのピクセルサイズ。
void DrawBossHealthBar(AtrumEngine *engine, float viewWidth, float viewHeight);

} // namespace Atrum::AtomF
