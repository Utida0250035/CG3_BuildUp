#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Atrum {

/// ステージビュー演出用（汎用 Ease とは別にカメラ体験向けに調整）
enum class StageViewCameraEaseKind : uint8_t {
  /// 互換用（従来の InOutQuad と同等）
  LegacyInOutQuad = 0,
  /// ボス紹介後の引き（序盤はほぼ据え置き、終盤で戦闘画へ収束）
  PullBackReveal = 1,
  /// フォーカスプッシュイン（素早く寄り、終端で滑らかに止まる）
  FocusPush = 2,
};

/// 序盤 35% は移動量 8% までに抑え、残りを smoothstep で繋ぐ
inline float StageViewCameraEasePullBackReveal(const float ratio) {
  const float t = std::clamp(ratio, 0.0f, 1.0f);
  if (t >= 1.0f) {
    return 1.0f;
  }
  if (t < 0.35f) {
    const float u = t / 0.35f;
    return 0.08f * u * u;
  }
  const float u = (t - 0.35f) / 0.65f;
  return 0.08f + 0.92f * (u * u * (3.0f - 2.0f * u));
}

/// 指数接近（t=1 で正確に 1）
inline float StageViewCameraEaseFocusPush(const float ratio) {
  constexpr float kApproach = 4.2f;
  const float t = std::clamp(ratio, 0.0f, 1.0f);
  if (t <= 0.0f) {
    return 0.0f;
  }
  if (t >= 1.0f) {
    return 1.0f;
  }
  const float denom = 1.0f - std::exp(-kApproach);
  return (1.0f - std::exp(-kApproach * t)) / denom;
}

inline float LegacyInOutQuad(const float ratio) {
  const float t = std::clamp(ratio, 0.0f, 1.0f);
  if (t < 0.5f) {
    return 2.0f * t * t;
  }
  return 1.0f - (-2.0f * t + 2.0f) * (-2.0f * t + 2.0f) / 2.0f;
}

inline float EvaluateStageViewCameraEase(const StageViewCameraEaseKind kind,
                                         const float ratio) {
  switch (kind) {
  case StageViewCameraEaseKind::LegacyInOutQuad:
    return LegacyInOutQuad(ratio);
  case StageViewCameraEaseKind::FocusPush:
    return StageViewCameraEaseFocusPush(ratio);
  case StageViewCameraEaseKind::PullBackReveal:
  default:
    return StageViewCameraEasePullBackReveal(ratio);
  }
}

} // namespace Atrum
