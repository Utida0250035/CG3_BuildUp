#pragma once

namespace Atrum {

class AtrumEngine;

/// 自機被弾時の画面端赤ヴィネット（中心へ向かって薄くなる）
class PlayerDamageScreenVignette {
public:
  static PlayerDamageScreenVignette &Instance();

  void Reset();

  /// 被弾演出を開始（表示中ならタイマーをリセット）
  void RequestShow();

  void Update(float deltaTimeSeconds);

  void Draw(AtrumEngine *engine);

private:
  float timer_ = 0.0f;
  float duration_ = 0.0f;
};

} // namespace Atrum
