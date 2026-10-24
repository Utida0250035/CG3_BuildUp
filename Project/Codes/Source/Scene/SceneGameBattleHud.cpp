#include "Scene/SceneGameBattleHud.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Platform/Window/Window.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/WhiteTexture.h"

#include <algorithm>

namespace Atrum {

namespace {

void DrawScreenRect(AtrumEngine *engine, const Math::Vector4 &color, float x,
                    float y, float width, float height) {
  Math::Transform uvTransform{};
  Math::Transform rectTransform{};
  rectTransform.translate = {x, y, 0.0f};
  rectTransform.scale = {width, height, 1.0f};

  engine->DrawSpriteRect(WhiteTextureIndex(), color, uvTransform, rectTransform,
                         Math::Vector2{1.0f, 1.0f}, Math::Vector2{0.0f, 0.0f});
}

void DrawPlayerHealthIcons(AtrumEngine *engine, const uint32_t health,
                           const uint32_t healthMax) {
  if (healthMax == 0) {
    return;
  }

  constexpr float kMarginLeft = 16.0f;
  constexpr float kMarginTop = 16.0f;
  constexpr float kIconSize = 28.0f;
  constexpr float kIconGap = 6.0f;
  const Math::Vector4 iconColor{0.35f, 0.9f, 1.0f, 1.0f};

  const uint32_t displayCount = health <= healthMax ? health : healthMax;
  for (uint32_t i = 0; i < displayCount; ++i) {
    const float x = kMarginLeft + static_cast<float>(i) * (kIconSize + kIconGap);
    DrawScreenRect(engine, iconColor, x, kMarginTop, kIconSize, kIconSize);
  }
}

void DrawBossHealthBar(AtrumEngine *engine, const uint32_t health,
                       const uint32_t healthMax, const float reveal01) {
  if (healthMax == 0) {
    return;
  }

  constexpr float kBarWidth = 560.0f;
  constexpr float kBarHeight = 20.0f;
  constexpr float kBottomMargin = 24.0f;

  const float barLeft =
      (static_cast<float>(Window::kDefaultClientWidth) - kBarWidth) * 0.5f;
  const float barTop =
      static_cast<float>(Window::kDefaultClientHeight) - kBottomMargin - kBarHeight;

  const Math::Vector4 frameColor{0.12f, 0.05f, 0.05f, 0.9f};
  DrawScreenRect(engine, frameColor, barLeft, barTop, kBarWidth, kBarHeight);

  const float ratio =
      static_cast<float>(health) / static_cast<float>(healthMax);
  const float reveal = std::clamp(reveal01, 0.0f, 1.0f);
  const float fillWidth =
      kBarWidth * std::clamp(ratio, 0.0f, 1.0f) * reveal;
  if (fillWidth <= 0.0f) {
    return;
  }

  const Math::Vector4 fillColor{0.92f, 0.12f, 0.1f, 1.0f};
  DrawScreenRect(engine, fillColor, barLeft, barTop, fillWidth, kBarHeight);
}

} // namespace

void DrawSceneGameBattleHud(AtrumEngine *engine, const uint32_t playerHealth,
                            const uint32_t playerHealthMax,
                            const uint32_t bossHealth,
                            const uint32_t bossHealthMax,
                            const float bossHpBarReveal01) {
  if (engine == nullptr) {
    return;
  }

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();

  DrawPlayerHealthIcons(engine, playerHealth, playerHealthMax);
  DrawBossHealthBar(engine, bossHealth, bossHealthMax, bossHpBarReveal01);
}

} // namespace Atrum
