#include "Camera/PlayerDamageScreenVignette.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Platform/Window/Window.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/WhiteTexture.h"

#include <algorithm>
#include <cmath>

namespace Atrum {

namespace {

constexpr float kVignetteDurationSeconds = 0.48f;
constexpr float kGradientDepthPixels = 128.0f;
constexpr int kGradientBandCount = 10;
constexpr float kEdgePeakAlpha = 0.52f;

void DrawScreenRect(AtrumEngine *engine, const Math::Vector4 &color, const float x,
                    const float y, const float width, const float height) {
  Math::Transform uvTransform{};
  Math::Transform rectTransform{};
  rectTransform.translate = {x, y, 0.0f};
  rectTransform.scale = {width, height, 1.0f};

  engine->DrawSpriteRect(WhiteTextureIndex(), color, uvTransform, rectTransform,
                         Math::Vector2{1.0f, 1.0f}, Math::Vector2{0.0f, 0.0f});
}

void DrawEdgeGradientBand(AtrumEngine *engine, const Math::Vector4 &baseColor,
                          const float alpha, const float x, const float y,
                          const float width, const float height) {
  if (alpha <= 0.001f) {
    return;
  }

  Math::Vector4 color = baseColor;
  color.w = alpha;
  DrawScreenRect(engine, color, x, y, width, height);
}

} // namespace

PlayerDamageScreenVignette &PlayerDamageScreenVignette::Instance() {
  static PlayerDamageScreenVignette instance;
  return instance;
}

void PlayerDamageScreenVignette::Reset() {
  timer_ = 0.0f;
  duration_ = 0.0f;
}

void PlayerDamageScreenVignette::RequestShow() {
  duration_ = kVignetteDurationSeconds;
  timer_ = kVignetteDurationSeconds;
}

void PlayerDamageScreenVignette::Update(const float deltaTimeSeconds) {
  if (timer_ <= 0.0f) {
    return;
  }

  timer_ -= deltaTimeSeconds;
  if (timer_ < 0.0f) {
    timer_ = 0.0f;
  }
}

void PlayerDamageScreenVignette::Draw(AtrumEngine *engine) {
  if (engine == nullptr || timer_ <= 0.0f || duration_ <= 0.0f) {
    return;
  }

  const float life01 = std::clamp(timer_ / duration_, 0.0f, 1.0f);
  const float intensity = life01 * life01;
  if (intensity <= 0.001f) {
    return;
  }

  const float screenWidth =
      static_cast<float>(Window::kDefaultClientWidth);
  const float screenHeight =
      static_cast<float>(Window::kDefaultClientHeight);
  const float bandHeight = kGradientDepthPixels /
                           static_cast<float>(kGradientBandCount);

  const Math::Vector4 baseColor{0.95f, 0.1f, 0.08f, 1.0f};

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();

  for (int band = 0; band < kGradientBandCount; ++band) {
    const float t = (static_cast<float>(band) + 0.5f) /
                    static_cast<float>(kGradientBandCount);
    const float falloff = (1.0f - t) * (1.0f - t);
    const float alpha = kEdgePeakAlpha * falloff * intensity;

    const float yTop = static_cast<float>(band) * bandHeight;
    DrawEdgeGradientBand(engine, baseColor, alpha, 0.0f, yTop, screenWidth,
                         bandHeight);

    const float yBottom =
        screenHeight - (static_cast<float>(band) + 1.0f) * bandHeight;
    DrawEdgeGradientBand(engine, baseColor, alpha, 0.0f, yBottom, screenWidth,
                         bandHeight);

    const float xLeft = static_cast<float>(band) * bandHeight;
    DrawEdgeGradientBand(engine, baseColor, alpha, xLeft, 0.0f, bandHeight,
                         screenHeight);

    const float xRight =
        screenWidth - (static_cast<float>(band) + 1.0f) * bandHeight;
    DrawEdgeGradientBand(engine, baseColor, alpha, xRight, 0.0f, bandHeight,
                         screenHeight);
  }
}

} // namespace Atrum
