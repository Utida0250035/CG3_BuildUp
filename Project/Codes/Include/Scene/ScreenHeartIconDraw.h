#pragma once

#include "Math/Vector4.h"

namespace Atrum {

class AtrumEngine;

/// 画面座標(ピクセル)にハート型アイコン 1 個を描く。HP 表示用。
/// 呼び出し側で PrepareSprite 済みであること。scale=1 のとき 28x28 の枠。
namespace ScreenHeartIcon {

inline constexpr float kSlotSize = 28.0f;

void Draw(AtrumEngine *engine, const Math::Vector4 &color, float originX,
          float originY, float scale = 1.0f);

} // namespace ScreenHeartIcon

} // namespace Atrum
