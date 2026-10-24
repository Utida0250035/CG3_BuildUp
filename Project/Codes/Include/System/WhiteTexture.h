#pragma once

#include "Engine/AtrumEngine.h"

namespace Atrum {

// 色だけの球や矩形に使う白テクスチャ。エンジン初期化後に呼ぶ
inline uint32_t WhiteTextureIndex() {
  static const uint32_t index =
      AtrumEngine::Instance()->GetTexture("./Assets/Images/white4x4.png");
  return index;
}

} // namespace Atrum
