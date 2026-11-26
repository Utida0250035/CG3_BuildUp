#pragma once

#include <string>

namespace Atrum {

class AtrumEngine;

namespace Script::UiSprite {

/// Lua の Ui.image が積む 2D スプライト 1 枚分（即時モード）
struct Entry {
  std::string texturePath{};
  /// 画面に対する中心位置（0〜1。0.5,0.5 が画面中央）
  float x = 0.5f;
  float y = 0.5f;
  float scale = 1.0f;
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
  float a = 1.0f;
};

/// 毎フレームの Update 先頭で前フレームの積み分を捨てる
void BeginFrame();

/// テクスチャを（必要なら）読み込んでキューへ積む。ファイルが無ければ無視する
void Push(const Entry &entry);

/// 積まれたスプライトを描画する（シーン描画の最後に呼ぶ）
void Draw(AtrumEngine *engine, float screenWidth, float screenHeight);

} // namespace Script::UiSprite
} // namespace Atrum
