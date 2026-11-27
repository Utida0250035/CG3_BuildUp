#pragma once

namespace Atrum {
class AtrumEngine;
} // namespace Atrum

namespace Atrum::AtomF {

/// 画面上部中央にボスの Hp バーを描く（ボスがいなければ何もしない）。
/// 激昂しきい値の目印と、激昂後・突進後の気絶中は色を変える。
void DrawBossHealthBar(AtrumEngine *engine, float viewWidth, float viewHeight);

} // namespace Atrum::AtomF
