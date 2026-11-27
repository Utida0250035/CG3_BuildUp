#pragma once

namespace Atrum {
class AtrumEngine;
}

namespace Atrum::AtomF {

/// 画面上部にボスの Hp バーを描く（ボスが居ない間は何も描かない）。
/// 登場演出中は左から伸び、被弾時は白い残像が遅れて縮む。
void DrawBossHealthBar(AtrumEngine *engine, float viewWidth, float viewHeight);

} // namespace Atrum::AtomF
