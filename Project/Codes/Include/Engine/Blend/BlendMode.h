#pragma once

#include <cstdint>

namespace Atrum {

enum class BlendMode : uint32_t {
  // ブレンド無し
  NONE,
  // 2値抜き(BinaryAlpha)
  BINARY_ALPHA,
  // 通常αブレンド
  NORMAL,
  // 加算
  ADD,
  // 減算
  SUBTRACT,
  // 乗算
  MULTIPLY,
  // スクリーン
  SCREEN,
  // モード数 使用禁止
  MODE_COUNT
};

}